// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class MULTIPLAYERCHALLENGE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar>HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>HealthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>AmmoAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>MatchCountDownText;

};
