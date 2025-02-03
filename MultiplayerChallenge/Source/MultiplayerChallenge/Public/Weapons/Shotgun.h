// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCHALLENGE_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget)override;

private:

	UPROPERTY(EditAnywhere, Category = "Weapon | Scatter")
	uint32 NumOfPellets = 10;
};
