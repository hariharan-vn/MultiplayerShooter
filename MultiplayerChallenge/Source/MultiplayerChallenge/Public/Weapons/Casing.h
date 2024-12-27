// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

class UStaticMeshComponent;
class USoundCue;

UCLASS()
class MULTIPLAYERCHALLENGE_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	ACasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent>CasingMesh;

	UPROPERTY(EditAnywhere, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	float ShellEjectionImpulse = 0;

	UPROPERTY(EditAnywhere, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundCue>ShellSound;
};
