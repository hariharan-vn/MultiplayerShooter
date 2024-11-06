#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;

UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void MoveForward(float Input);
	virtual void MoveSide(float Input);
	virtual void Turn(float Input);
	virtual void LookUp(float Input);

	virtual void EquipButtonPressed();
	virtual void CrouchButtonPressed();
	virtual void AimButtonPressed();
	virtual void AimButtonReleased();

	virtual void CalculateAimOffset(float DeltaTime);

public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents()override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent>CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<UCombatComponent>Combat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext>BlasterInputContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction>MovementInput;

	UPROPERTY(EditAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent>OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon>OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float AO_Pitch;

	FRotator StartingAimRotation;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped()const;

	bool IsAiming()const;

	FORCEINLINE float Get_AO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float Get_AO_Pitch() const { return AO_Pitch; }
	FORCEINLINE AWeapon* GetEquippedWeapon()const;

protected:

	void EnhancedInputMove(const FInputActionValue& Value);

};
