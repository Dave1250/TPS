// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "KillEveryoneGameMode.h"
#include "Kismet/GameplayStatics.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	HUD = CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

void ATPSPlayerController::GameHasEnded(class AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
	
	if (bIsWinner)
	{
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);
		if (WinScreen != nullptr)
		{
			HUD->RemoveFromViewport();
			WinScreen->AddToViewport();
			GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
		}
	}
	else
	{
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);
		HUD->RemoveFromViewport();
		if (LoseScreen != nullptr)
		{
			LoseScreen->AddToViewport();
			GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
		}
	}
}