// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;
class UTexture2D;
class ABlasterCharacter;
class ABlasterPlayerController;
class USoundCue;

UENUM(BlueprintType)
enum EWeaponState :uint8
{
	EWS_Initial		UMETA(DisplayName = "Initial"),
	EWS_Equipped	UMETA(DisplayName = "Equipped"),
	EWS_Dropped		UMETA(DisplayName = "Dropped"),
	EWS_MAX			UMETA(DisplayName = "MAX")
};

UCLASS()
class MULTIPLAYERCHALLENGE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;

	void ShowPickupWidget(bool bShowWidget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnRep_Owner()override;

	void SetHUDAmmo();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent>WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent>AreaSphere = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	TEnumAsByte<EWeaponState> WeaponState = EWeaponState::EWS_Initial;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent >PickupWidget;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<ACasing>CasingClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Weapon Properties")
	int32 Ammo;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 MagCapacity;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();

	UPROPERTY()
	TObjectPtr<ABlasterCharacter>BlasterOwnerCharacter;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController>BlasterOwnerController;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;

public:
	/*
		Automatic Fire
	*/

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bAutomatic = true;

	//Textures for weapon crosshairs

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Crosshairs")
	TObjectPtr<UTexture2D>CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Crosshairs")
	TObjectPtr<UTexture2D>CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Crosshairs")
	TObjectPtr<UTexture2D>CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Crosshairs")
	TObjectPtr<UTexture2D>CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Crosshairs")
	TObjectPtr<UTexture2D>CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Zoom")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Zoom")
	float ZoomInterpSpeed = 20.f;

	void SetWeaponState(EWeaponState _State);
	FORCEINLINE USphereComponent* GetAreaSphere()const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh()const { return WeaponMesh; }

	FORCEINLINE float GetZoomedFOV()const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed()const { return ZoomInterpSpeed; }

	virtual void Fire(const FVector& HitTarget);

	void Dropped();

	bool IsEmpty()const;

	void AddAmmo(int32 AmmoToAdd);

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE int32 GetAmmo()const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity()const { return MagCapacity; }

	UPROPERTY(EditAnywhere, Category = "Weapon Properties | Equip")
	TObjectPtr<USoundCue> EquipSound;

};
