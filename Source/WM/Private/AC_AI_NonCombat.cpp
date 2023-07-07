// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_NonCombat.h"
#include "AI_EnemyBase.h"
#include "AIModule/Classes/AIController.h"
#include "../WM.h"
#include "../../../../UE_5.2/Engine/Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
// Sets default values for this component's properties
UAC_AI_NonCombat::UAC_AI_NonCombat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_AI_NonCombat::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerEnemy = Cast<AAI_EnemyBase>(GetOwner());
}


// Called every frame
void UAC_AI_NonCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerEnemy && !OwnerEnemy->bIsBattle)
	{
		StateTimer = FMath::Max(0, StateTimer - DeltaTime);
		if (StateTimer == 0)
		{
			StateTimer = FMath::RandRange(StateTimerMin, StateTimerMax);
			SetRandomPoint();
		}
	}
}

void UAC_AI_NonCombat::SetRandomPoint()
{
	if (pointArray.IsEmpty()) return;
	int randomint = FMath::RandRange(0, pointArray.Num() - 1);
	AActor* temppoint = currentPoint;
	currentPoint = pointArray[randomint];
	FVector loc = currentPoint->GetActorLocation();
	if (OwnerEnemy->aicontroller)
	{;
		OwnerEnemy->aicontroller->MoveToLocation(loc);
	}
}

void UAC_AI_NonCombat::StateChange(ENONCOMBAT ChageState)
{
	State = ChageState;
	OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 200;
	switch (State)
	{
	case ENONCOMBAT::IDLE:
		break;
	case ENONCOMBAT::MOVE:
		break;
	default:
		break;
	}
}