#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class AWeapon;

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

protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bAimingStatus);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bAimingStatus);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	TObjectPtr<ABlasterCharacter>Character;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon>EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming = false;

	UPROPERTY(EditAnywhere, Category = "Character|Combat Settings")
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Character|Combat Settings")
	float AimWalkSpeed;
};
