
#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = BlasterCharacter ? BlasterCharacter : Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	BlasterCharacter = BlasterCharacter ? BlasterCharacter : Cast<ABlasterCharacter>(TryGetPawnOwner());

	if (!BlasterCharacter)return;

	FVector CharVel = BlasterCharacter->GetVelocity();
	Speed = CharVel.Size2D();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0 ? true : false;
}
