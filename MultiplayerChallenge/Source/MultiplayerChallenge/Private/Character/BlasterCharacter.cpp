
#include "Character/BlasterCharacter.h"
#include "Weapons/Weapon.h"

#include "Components/CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerChallenge/MultiplayerChallenge.h"
#include "PlayerController/BlasterPlayerController.h"
#include "Gamemodes/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapons/WeaponTypes.h"
#include "PlayerState/BlasterPlayerState.h"

ABlasterCharacter::ABlasterCharacter() :AO_Yaw(0.f), AO_Pitch(0.f), StartingAimRotation(FRotator::ZeroRotator), TurningInPlace(ETurningInPlace::ETIP_NotTurning)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->SetupAttachment(GetMesh());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);

	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	//Standard values for fast paced shooter games
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>("DissolveTimeline");

}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ABlasterCharacter, Health);
	DOREPLIFETIME(ABlasterCharacter, bDisableGameplay);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PIC)
{
	Super::SetupPlayerInputComponent(PIC);

	PIC->BindAxis("MoveForward", this, &ThisClass::MoveForward);
	PIC->BindAxis("MoveSide", this, &ThisClass::MoveSide);
	PIC->BindAxis("Turn", this, &ThisClass::Turn);
	PIC->BindAxis("LookUp", this, &ThisClass::LookUp);

	PIC->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PIC->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PIC->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PIC->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PIC->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);
	PIC->BindAction("Fire", IE_Pressed, this, &ThisClass::FireButtonPressed);
	PIC->BindAction("Fire", IE_Released, this, &ThisClass::FireButtonReleased);
	PIC->BindAction("Reload", IE_Pressed, this, &ThisClass::ReloadButtonPressed);

	auto EIC = Cast<UEnhancedInputComponent>(PIC);

	if (EIC)
	{
		EIC->BindAction(MovementInput, ETriggerEvent::Triggered, this, &ThisClass::EnhancedInputMove);
	}
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastReplication = 0.f;
}

void ABlasterCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ThisClass::ElimTimerFinished,
		ElimDelay);
}

void ABlasterCharacter::MulticastElim_Implementation()
{
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDWeaponAmmo(0);
	}

	bElimmed = true;
	PlayElimMontage();

	//Start Dissolve Effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	//Disable Character Movement
	GetCharacterMovement()->DisableMovement();//stops movement 
	GetCharacterMovement()->StopMovementImmediately();//stop mouse control

	bDisableGameplay = true;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	//Disable Collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Spawn Elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPosition(GetActorLocation());
		ElimBotSpawnPosition.Z += 200.f;//on top of the character head

		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPosition,
			GetActorRotation()
		);

		if (ElimBotSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				GetWorld(),
				ElimBotSound,
				GetActorLocation()
			);
		}
	}
}

void ABlasterCharacter::Destroyed()
{
	Super::Destroyed();

	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	auto BlasterGM = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = BlasterGM && BlasterGM->GetMatchState() != MatchState::InProgress;

	//Destroying weapons on transition levels
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void ABlasterCharacter::ElimTimerFinished()
{
	if (auto BlasterGM = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
	{
		BlasterGM->RequestRespawn(this, Controller);
	}
}

void ABlasterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	auto PC = Cast<APlayerController>(NewController);
	if (PC)
	{
		auto EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (EILPS)
		{
			EILPS->AddMappingContext(BlasterInputContext, 0);
		}
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ABlasterCharacter::OnRep_Health()
{
	PlayHitReactMontage();
	UpdateHUDHealth();
}

void ABlasterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	PlayHitReactMontage();
	UpdateHUDHealth();

	if (Health <= 0.f)
	{
		if (auto BlasterGM = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
		{
			auto AttackerController = Cast<ABlasterPlayerController>(InstigatorController);
			BlasterGM->PlayerEliminated(this, BlasterPlayerController, AttackerController);
		}
	}
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}

}

void ABlasterCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABlasterCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
		OverlappingWeapon->ShowPickupWidget(false);
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
			OverlappingWeapon->ShowPickupWidget(true);
	}
}

ECombatState ABlasterCharacter::GetCombatState() const
{
	if (Combat)return Combat->CombatState;

	return ECombatState::ECS_MAX;
}

void ABlasterCharacter::EnhancedInputMove(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	float IgnoreThreshold = 0.05f;

	if (MoveVector.X > IgnoreThreshold && MoveVector.X < -IgnoreThreshold)
	{
		MoveForward(MoveVector.X);
	}
	if (MoveVector.Y > IgnoreThreshold && MoveVector.Y < -IgnoreThreshold)
	{
		MoveSide(MoveVector.Y);
	}
}

void ABlasterCharacter::MoveForward(float Input)
{
	if (bDisableGameplay)return;

	if (Controller && Input != 0.f)
	{
		FRotator YawRotation(FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f));
		FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Input);
	}
}

void ABlasterCharacter::MoveSide(float Input)
{
	if (bDisableGameplay)return;

	if (Controller && Input != 0.f)
	{
		FRotator YawRotation(FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f));
		FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Input);
	}
}

void ABlasterCharacter::Turn(float Input)
{
	if (Input != 0.f)
		AddControllerYawInput(Input);
}

void ABlasterCharacter::LookUp(float Input)
{
	if (Input != 0.f)
	{
		AddControllerPitchInput(Input);
		UE_LOG(LogTemp, Warning, TEXT("LookUp %f"), Input);
	}
}

void ABlasterCharacter::EquipButtonPressed()
{
	if (bDisableGameplay)return;

	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABlasterCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay)return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABlasterCharacter::AimButtonPressed()
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void ABlasterCharacter::AimButtonReleased()
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void ABlasterCharacter::FireButtonPressed()
{
	if (bDisableGameplay)return;

	if (!Combat)return;

	Combat->FireButtonPressed(true);
}

void ABlasterCharacter::FireButtonReleased()
{
	if (bDisableGameplay)return;

	if (!Combat)return;

	Combat->FireButtonPressed(false);
}

void ABlasterCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay)return;

	if (!Combat)return;

	Combat->Reload();
}

bool ABlasterCharacter::IsWeaponEquipped()const
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()const
{
	return (Combat && Combat->bAiming);
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (!Combat || !Combat->EquippedWeapon)return;

	auto AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && FireWeaponMontage)
	{
		AnimInst->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? "RifleAim" : "RifleHip";
		AnimInst->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayElimMontage()
{
	auto AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && ElimMontage)
	{
		AnimInst->Montage_Play(ElimMontage);
	}
}

void ABlasterCharacter::PlayReloadMontage()
{
	if (!Combat || !Combat->EquippedWeapon)return;

	auto AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && ReloadMontage)
	{
		AnimInst->Montage_Play(ReloadMontage);
		FName SectionName("");
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = "Rifle";
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = "Rifle";
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = "Rifle";
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = "Rifle";
			break;
		case EWeaponType::EWT_ShotGun:
			SectionName = "Rifle";
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = "Rifle";
			break;
		}

		AnimInst->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::PlayHitReactMontage()
{
	if (!Combat || !Combat->EquippedWeapon)return;

	auto AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && HitReactMontage)
	{
		AnimInst->Montage_Play(HitReactMontage);
		FName SectionName = "FromFront";// : "RifleHip";
		AnimInst->Montage_JumpToSection(SectionName);
	}
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	if (!Combat)return nullptr;

	return Combat->EquippedWeapon;
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);

	HideCameraIfCharacterClose();

	PollInit();
}

float ABlasterCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	return Velocity.Size2D();
}

void ABlasterCharacter::CalculateAimOffset(float DeltaTime)
{
	//No Weapon No Aim Offset can be performed
	if (Combat && !Combat->EquippedWeapon)return;

	float Speed = CalculateSpeed();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	//Standing Still and not jumping
	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;

		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
			//UE_LOG(LogTemp, Warning, TEXT("InterpAO_Yaw is assigned with value %f"), InterpAO_Yaw);
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	//running or jumping
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;

		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}
	CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{

	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270,360) to [-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(270.f, 360.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABlasterCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled())return;

	bool bIsClose = (FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraDistThreshold;

	GetMesh()->SetVisibility(!bIsClose);
	if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
	{
		Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = bIsClose;
	}
}

void ABlasterCharacter::Jump()
{
	if (bDisableGameplay)return;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::SimProxiesTurn()
{
	if (!Combat || !Combat->EquippedWeapon)return;

	bRotateRootBone = false;

	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw - 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	//Either Turning Left or Right
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw  is assigned with value %f"), InterpAO_Yaw);

		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
			//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw  is assigned with value %s"), *(StartingAimRotation.ToString()));
		}
	}
}

FVector ABlasterCharacter::GetHitTarget()const
{
	if (!Combat)return FVector();

	return Combat->HitTargetVector;
}

void ABlasterCharacter::UpdateHUDHealth()
{
	BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(Controller) : BlasterPlayerController;
	if (BlasterPlayerController)
	{
		BlasterPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ABlasterCharacter::PollInit()
{
	if (!BlasterPlayerState)
	{
		BlasterPlayerState = Cast<ABlasterPlayerState>(GetPlayerState());
		if (BlasterPlayerState)
		{
			BlasterPlayerState->AddToScore(0.f);
			BlasterPlayerState->AddToDefeats(0);
		}
	}
}

void ABlasterCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		CalculateAimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastReplication += DeltaTime;
		float ReplicationThresholdTimer = 0.25f;
		//waiting to see if it called for 0.25 sec if not calling it manually as turn in place animation need to be synced 
		if (TimeSinceLastReplication > ReplicationThresholdTimer)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}
