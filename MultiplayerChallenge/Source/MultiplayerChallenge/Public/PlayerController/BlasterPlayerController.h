// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;
class ABlasterGameMode;

UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay()override;

	void SetHUDTime();

	void PollInit();

	/*
	* Sync Time Between Server and Client
	*/

	//Request the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//Report the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	//Difference between client and server time
	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "Server Time")
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

private:

	void CheckTimeSync(float DeltaTime);

	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountDownInt;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay = false;
	float HUDHealth = 0.f;
	float HUDMaxHealth = 0.f;
	float HUDScore = 0.f;
	int32 HUDDefeats = 0;

public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCountDown(float CountDownTime);
	void SetHUDAnnouncementCountdown(float AnnouncementTime);

	virtual void OnPossess(APawn* InPawn)override;
	virtual float GetServerTime();//Synced with server world clock


	virtual void Tick(float DeltaTime)override;
	virtual void ReceivedPlayer()override;//Sync with server clock as soon as possible

	void OnMatchStateSet(FName State);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void HandleMatchHasStarted();
	void HandleCooldown();
};
