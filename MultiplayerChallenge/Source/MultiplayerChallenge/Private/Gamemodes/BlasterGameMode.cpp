#include "Gamemodes/BlasterGameMode.h"

#include "Character/BlasterCharacter.h"
#include "PlayerController/BlasterPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

#include "PlayerState/BlasterPlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	auto AttackerPS = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	auto VictimPS = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPS && AttackerPS != VictimPS)
	{
		AttackerPS->AddToScore(1.f);
	}
	if (VictimPS)
	{
		VictimPS->AddToDefeats(1);
	}
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*>PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
