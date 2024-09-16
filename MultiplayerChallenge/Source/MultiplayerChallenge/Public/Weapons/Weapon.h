// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;

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

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent>WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent>AreaSphere = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState = EWeaponState::EWS_Initial;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent >PickupWidget;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent OnComponentBeginOverlap, UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
