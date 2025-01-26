// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "ProjectileRocket.generated.h"

class UStaticMeshComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class URocketMovementComponent;

UCLASS()
class MULTIPLAYERCHALLENGE_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileRocket();

	virtual void Destroyed()override;

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;

	virtual void BeginPlay()override;

	UFUNCTION()
	void DestoyTimerFinished();

	UPROPERTY(EditAnywhere, Category = "Ammo Properties| FX")
	TObjectPtr<UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties| FX")
	TObjectPtr<USoundCue>ProjectileLoop;

	UPROPERTY()
	TObjectPtr<UAudioComponent>ProjectileLoopComponent;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties| FX")
	TObjectPtr<USoundAttenuation>LoopingSoundAttenuation;

	UPROPERTY()
	TObjectPtr<URocketMovementComponent>RocketMovementComponent;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Ammo Properties")
	TObjectPtr<UStaticMeshComponent> RocketMesh;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties| FX")
	float DestroyTime = 3.f;

	FTimerHandle DestroyTimer;
};
