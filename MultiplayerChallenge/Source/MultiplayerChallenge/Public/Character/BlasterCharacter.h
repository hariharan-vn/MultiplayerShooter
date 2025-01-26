#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class ABlasterPlayerController;
class UParticleSystem;
class USoundCue;
class ABlasterPlayerState;

UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
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
	virtual void FireButtonPressed();
	virtual void FireButtonReleased();
	virtual void ReloadButtonPressed();

	virtual void CalculateAimOffset(float DeltaTime);

	void CalculateAO_Pitch();

	virtual void Jump()override;

	void SimProxiesTurn();

public:

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents()override;

	virtual void OnRep_ReplicatedMovement()override;

	virtual void Destroyed()override;

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent>CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Camera")
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
	float InterpAO_Yaw;
	float AO_Pitch;

	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void TurnInPlace(float DeltaTime);

	/*
		Animation Montages
	*/

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Camera")
	float CameraDistThreshold = 200.f;

	void HideCameraIfCharacterClose();
	void PlayHitReactMontage();

	bool bRotateRootBone = false;

	float TurnThreshold = 0.5f;
	float ProxyYaw = 0.f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float TimeSinceLastReplication = 0.f;

	float CalculateSpeed();

	/*
		Player Health
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Player Stats")
	float Health = 0.f;

	UFUNCTION()
	void OnRep_Health();

	ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly, Category = "Player Elimination", meta = (AllowPrivateAccess = "true"))
	float ElimDelay = 3.f;

	UFUNCTION()
	void ElimTimerFinished();

	/*
	Dissolve Effect
	*/
	UPROPERTY(VisibleAnywhere, Category = "Dissolve Effect")
	TObjectPtr<UTimelineComponent>DissolveTimeline;

	FOnTimelineFloat DissolveTrack;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(EditAnywhere, Category = "Dissolve Effect")
	TObjectPtr<UCurveFloat>DissolveCurve;

	//dynamic instance which we can change at the runtime
	UPROPERTY(VisibleAnywhere, Category = "Dissolve Effect")
	TObjectPtr<UMaterialInstanceDynamic>DynamicDissolveMaterialInstance;

	//material instance which is set on blueprint, used with dynamic material instance 
	UPROPERTY(EditAnywhere, Category = "Dissolve Effect")
	TObjectPtr<UMaterialInstance>DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = "ElimBot")
	TObjectPtr<UParticleSystem> ElimBotEffect;

	UPROPERTY(VisibleAnywhere, Category = "ElimBot")
	TObjectPtr<UParticleSystemComponent> ElimBotComponent;

	UPROPERTY(EditAnywhere, Category = "ElimBot")
	TObjectPtr<USoundCue>ElimBotSound;

	TObjectPtr< ABlasterPlayerState>BlasterPlayerState;

public:

	void SetOverlappingWeapon(AWeapon* Weapon);

	bool IsWeaponEquipped()const;

	bool IsAiming()const;

	void PlayFireMontage(bool bAiming);

	void PlayElimMontage();

	void PlayReloadMontage();

	FORCEINLINE float Get_AO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float Get_AO_Pitch() const { return AO_Pitch; }
	FORCEINLINE AWeapon* GetEquippedWeapon()const;
	FORCEINLINE ETurningInPlace GetTuringInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera()const { return FollowCamera; }

	FVector GetHitTarget()const;

	FORCEINLINE bool ShouldRotateRootBone()const { return bRotateRootBone; }

	FORCEINLINE bool IsElimmed()const { return bElimmed; }

	FORCEINLINE float GetHealth()const { return Health; }
	FORCEINLINE float GetMaxHealth()const { return MaxHealth; }

	ECombatState GetCombatState()const;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	FORCEINLINE UCombatComponent* GetCombat()const { return Combat; }

	FORCEINLINE bool GetDisableGameplay()const { return bDisableGameplay; }

protected:

	void EnhancedInputMove(const FInputActionValue& Value);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	void UpdateHUDHealth();

	//Poll for relevant classes and initialize for HUD
	void PollInit();

	void RotateInPlace(float DeltaTime);
};
