// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	//Rep Notifies
	UFUNCTION()
	virtual void OnRep_Defeats();
	virtual void OnRep_Score()override;

private:
	UPROPERTY()
	TObjectPtr<ABlasterCharacter>Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
};
