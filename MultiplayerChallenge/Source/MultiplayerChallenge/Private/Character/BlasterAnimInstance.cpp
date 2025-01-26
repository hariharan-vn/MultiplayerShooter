
#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "Weapons/Weapon.h"
#include "BlasterTypes/CombatState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();

	bIsCrouched = BlasterCharacter->bIsCrouched;

	bAiming = BlasterCharacter->IsAiming();

	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

	TurningInPlace = BlasterCharacter->GetTuringInPlace();

	bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

	bElimmed = BlasterCharacter->IsElimmed();

	bUseFabric = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;

	bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading&& !BlasterCharacter->GetDisableGameplay();
	bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading&& !BlasterCharacter->GetDisableGameplay();

	//Offset Yaw for Strafing
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 15.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.0f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = BlasterCharacter->Get_AO_Yaw();
	AO_Pitch = BlasterCharacter->Get_AO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(OutRotation.Quaternion());

		bIsLocallyControlled = BlasterCharacter->IsLocallyControlled();
		if (bIsLocallyControlled)
		{
			//socket names are not case sensitive so we can use hand_r or Hand_R, both does the same
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation= UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 30.f);
		}

		FTransform MuzzleTipTransform(EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World));
		FVector Muzzle(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
		FVector StartLoc = MuzzleTipTransform.GetLocation();
		DrawDebugLine(GetWorld(), StartLoc, StartLoc + Muzzle * 1000.f, FColor::Red);
		DrawDebugLine(GetWorld(), StartLoc, BlasterCharacter->GetHitTarget(), FColor::Orange);
	}
}
