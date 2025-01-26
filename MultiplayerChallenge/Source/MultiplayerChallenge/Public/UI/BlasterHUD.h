// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UTexture2D;
class UCharacterOverlay;
class UAnnouncement;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	TObjectPtr<UTexture2D>CrosshairCenter = nullptr;
	TObjectPtr<UTexture2D>CrosshairLeft = nullptr;
	TObjectPtr<UTexture2D>CrosshairRight = nullptr;
	TObjectPtr<UTexture2D>CrosshairTop = nullptr;
	TObjectPtr<UTexture2D>CrosshairBottom = nullptr;
	FLinearColor CrosshairColor;
	float Spread = 0.f;
};


UCLASS()
class MULTIPLAYERCHALLENGE_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD()override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UCharacterOverlay>CharacterOverlayClass;

	UPROPERTY()
	TObjectPtr<UCharacterOverlay>CharacterOverlay;

	void AddCharacterOverlay();


	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UAnnouncement>AnnouncementClass;

	UPROPERTY()
	TObjectPtr<UAnnouncement>Announcement;

	void AddAnnouncement();

	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

	UPROPERTY(EditAnywhere, Category = "HUD")
	float CrossHairSpreadMax = 16.f;


protected:
	virtual void BeginPlay()override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

};
