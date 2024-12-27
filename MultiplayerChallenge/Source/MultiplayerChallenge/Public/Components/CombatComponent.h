#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI/BlasterHUD.h"
#include "Weapons/WeaponTypes.h"
#include "BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class AWeapon;
class ABlasterHUD;
class ABlasterPlayerController;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAYERCHALLENGE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend ABlasterCharacter;

	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bAimingStatus);

	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bAimingStatus);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& HitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& HitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrossHairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();

private:
	TObjectPtr<ABlasterCharacter>Character;
	TObjectPtr<ABlasterPlayerController>Controller;
	TObjectPtr<ABlasterHUD>HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon>EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming = false;

	UPROPERTY(EditAnywhere, Category = "Character|Combat Settings")
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Character|Combat Settings")
	float AimWalkSpeed;

	bool bFireButtonPressed = false;

	/*
	* HUD and Crosshair
	*/
	float CrosshairVelocityFactor = 0.f;
	float CrosshairInAirFactor = 0.f;
	float CrosshairAimFactor = 0.f;
	float CrosshairShootingFactor = 0.f;

	FVector HitTargetVector;

	FHUDPackage Package;

	/*
	* Aiming and FOV
	*/

	// FOV when not aiming, set to the base FOV
	float DefaultFOV;
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat | Camera")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Combat | Camera")
	float ZoomInterpSpeed = 30.f;

	void InterpFOV(float DeltaTime);

	/*
	*	Automatic Fire
	*/

	FTimerHandle FireTimer;

	bool bCanFire = true;

	void StartFireTimer();

	UFUNCTION()
	void FireTimerFinished();

	void Fire();

	bool CanFire()const;

	//Carried Ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32>CarriedAmmoMap;

	void InitializeCarriedAmmo();

	UPROPERTY(EditAnywhere, Category = "Combat | Weapon")
	int32 StartingARAmmo = 30;

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
};