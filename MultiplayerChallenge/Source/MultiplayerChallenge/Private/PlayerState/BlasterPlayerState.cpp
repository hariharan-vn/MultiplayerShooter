// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/BlasterPlayerState.h"

#include "Character/BlasterCharacter.h"
#include "PlayerController/BlasterPlayerController.h"

#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Defeats()
{
	if (Character == nullptr)
	{
		Character = Cast<ABlasterCharacter>(GetPawn());
	}
	if (Controller == nullptr)
	{
		Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}
	else
	{
		Controller->SetHUDDefeats(Defeats);
	}
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	if (Character == nullptr)
	{
		Character = Cast<ABlasterCharacter>(GetPawn());
	}
	if (Controller == nullptr)
	{
		Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}
	else
	{
		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	if (Character == nullptr)
	{
		Character = Cast<ABlasterCharacter>(GetPawn());
	}

	if (Controller == nullptr)
	{
		Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}
	else
	{
		Controller->SetHUDScore(GetScore());
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	if (Character == nullptr)
	{
		Character = Cast<ABlasterCharacter>(GetPawn());
	}
	if (Controller == nullptr)
	{
		Controller = Cast<ABlasterPlayerController>(Character->Controller);
	}
	else
	{
		Controller->SetHUDDefeats(Defeats);
	}
}
