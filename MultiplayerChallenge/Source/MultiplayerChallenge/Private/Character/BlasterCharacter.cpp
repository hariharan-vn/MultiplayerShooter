
#include "Character/BlasterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->SetupAttachment(GetMesh());

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EIC)
	{
		EIC->BindAction(MovementInput, ETriggerEvent::Triggered, this, &ThisClass::EnhancedInputMove);
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
		MoveForward(MoveVector.Y);
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
	AddControllerYawInput(Input);
}

void ABlasterCharacter::LookUp(float Input)
{
	AddControllerPitchInput(Input);
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


