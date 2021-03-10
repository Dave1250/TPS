// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSGameModeBase.h"
#include "KillEveryoneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TPS_API AKillEveryoneGameMode : public ATPSGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PawnKilled(APawn* PawnKilled) override;
	
private:

	void GameEnd(bool bHasPlayerWon);

};
