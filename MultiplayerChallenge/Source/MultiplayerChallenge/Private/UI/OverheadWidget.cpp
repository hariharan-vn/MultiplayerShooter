// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OverheadWidget.h"
#include "Components/TextBlock.h"


void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (!DisplayText)return;

	DisplayText->SetText(FText::FromString(TextToDisplay));
}


void UOverheadWidget::ShowPlayerNetRole(APawn* Pawn)
{
	auto Role = Pawn->GetLocalRole();

	FString RoleString = "Local Role ";
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		RoleString += "Authority";
		break;
	case ENetRole::ROLE_AutonomousProxy:
		RoleString += "AutonomousProxy";
		break;
	case ENetRole::ROLE_SimulatedProxy:
		RoleString += "SimulatedProxy";
		break;
	case ENetRole::ROLE_None:
		RoleString += "None";
		break;
	}

	SetDisplayText(RoleString);
}


void UOverheadWidget::NativeDestruct()
{

}
