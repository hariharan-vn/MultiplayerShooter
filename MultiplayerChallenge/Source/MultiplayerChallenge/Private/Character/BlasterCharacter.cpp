
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

ABlasterCharacter::ABlasterCharacter() :AO_Yaw(0.f), AO_Pitch(0.f), StartingAimRotation(FRotator::ZeroRotator)
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
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
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
	if (Controller && Input != 0.f)
	{
		FRotator YawRotation(FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f));
		FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Input);
	}
}

void ABlasterCharacter::MoveSide(float Input)
{
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

bool ABlasterCharacter::IsWeaponEquipped()const
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming()const
{
	return (Combat && Combat->bAiming);
}

AWeapon* ABlasterCharacter::GetEquippedWeapon() const
{
	if (!Combat)return nullptr;

	return Combat->EquippedWeapon;
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateAimOffset(DeltaTime);
}

void ABlasterCharacter::CalculateAimOffset(float DeltaTime)
{
	//No Weapon No Aim Offset can be performed
	if (Combat && !Combat->EquippedWeapon)return;

	FVector Velocity = GetVelocity();
	float Speed = Velocity.Size2D();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	//Standing Still and not jumping
	if (Speed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
	}
	//running or jumping
	if (Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270,360) to [-90,0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(270.f, 360.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

