// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystem;
/**
 *
 */
UCLASS()
class MULTIPLAYERCHALLENGE_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	virtual void Fire(const FVector& HitTarget);

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> HitSound;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> BeamParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> FireSound;

	/// Trace End with Scatter

	UPROPERTY(EditAnywhere, Category = "Weapon | Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = "Weapon | Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = "Weapon | Scatter")
	bool bUseScatter = false;

};
