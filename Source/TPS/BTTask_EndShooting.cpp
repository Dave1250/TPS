// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EndShooting.h"
#include "AIController.h"
#include "ShooterCharacter.h"

UBTTask_EndShooting::UBTTask_EndShooting()
{
	NodeName = "Stop Shooting";
}

EBTNodeResult::Type UBTTask_EndShooting::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AShooterCharacter* Character = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	Character->StopShooting();

	return EBTNodeResult::Succeeded;
}