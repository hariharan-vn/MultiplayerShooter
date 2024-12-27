// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class MULTIPLAYERCHALLENGE_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed()override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Ammo Properties")
	float Damage = 20.f;

private:
	UPROPERTY(EditAnywhere, Category = "Ammo Collider", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent>CollisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Ammo Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent>ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem>Tracer;

	TObjectPtr<UParticleSystemComponent>TracerComponent;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem>ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Ammo Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundCue>ImpactSound;

};
