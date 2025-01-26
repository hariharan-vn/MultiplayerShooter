// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;

namespace MatchState
{
	extern MULTIPLAYERCHALLENGE_API const FName Cooldown;//Match duration has been reached. Display winner and begin cooldown timer
}


UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterGameMode();

	virtual void PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	virtual void Tick(float DeltaTime)override;

	UPROPERTY(EditDefaultsOnly, Category = "Match Time")
	float WarmupTime = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Match Time")
	float MatchTime = 120.f;
	UPROPERTY(EditDefaultsOnly, Category = "Match Time")
	float CooldownTime = 0.f;

	float LevelStartingTime = 0.f;

	FORCEINLINE float GetCountdownTime()const { return CountdownTime; }

private:
	float CountdownTime = 0.f;

protected:
	virtual void BeginPlay()override;
	virtual void OnMatchStateSet()override;

};
