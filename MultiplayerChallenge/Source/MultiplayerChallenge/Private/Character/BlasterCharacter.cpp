
#include "Character/BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
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

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

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

	auto EIC = Cast<UEnhancedInputComponent>(PIC);

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

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


