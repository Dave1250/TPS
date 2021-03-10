// Fill out your copyright notice in the Description page of Project Settings.


#include "KillEveryoneGameMode.h"
#include "ShooterCharacter.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "ShooterAIController.h"
#include "GameFramework/Controller.h"


void AKillEveryoneGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());

	if (PlayerController != nullptr)
	{
		GameEnd(false);
	}

	for (AShooterAIController* Controller : TActorRange<AShooterAIController>(GetWorld()))
	{
		if (!Controller->IsDead())
		{
			return;
		}
	}

	GameEnd(true);
}

void AKillEveryoneGameMode::GameEnd(bool bHasPlayerWon)
{
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == bHasPlayerWon;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}