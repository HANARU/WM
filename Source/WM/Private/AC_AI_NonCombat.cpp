// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_NonCombat.h"
#include "AI_EnemyBase.h"
#include "AIModule/Classes/AIController.h"
#include "../WM.h"
#include "../../../../UE_5.2/Engine/Source/Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "AI_EnemyAnimInstance.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"
// Sets default values for this component's properties
UAC_AI_NonCombat::UAC_AI_NonCombat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	pointtagindex = -4;
	// ...
}


// Called when the game starts
void UAC_AI_NonCombat::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerEnemy = Cast<AAI_EnemyBase>(GetOwner());
	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor)
		{
			if (Actor->ActorHasTag(FName(FString::Printf(TEXT("patrol%d"), pointtagindex))))
			{
				pointArray.Add(Actor);
			}
		}
	}
}


// Called every frame
void UAC_AI_NonCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (OwnerEnemy->bIsdie) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerEnemy && !OwnerEnemy->bIsBattle)
	{
		StateTimer = FMath::Max(0, StateTimer - DeltaTime);
		if (StateTimer == 0)
		{
			StateTimer = FMath::RandRange(StateTimerMin, StateTimerMax);
			SetRandomPoint();
		}
		switch (State)
		{
		case ENONCOMBAT::IDLE:
			StateIdle();
			break;
		case ENONCOMBAT::MOVE:
			StateMove();
			break;
		default:
			break;
		}
	}
}

void UAC_AI_NonCombat::SetRandomPoint()
{
	if (pointArray.IsEmpty()) return;
	if(FMath::RandBool())
	{
		OwnerEnemy->aicontroller->StopMovement();
		StateChange(ENONCOMBAT::IDLE);
	}
	else
	{
		if (currentPoint == nullptr)
		{
			int randomint = FMath::RandRange(0, pointArray.Num() - 1);
			AActor* temppoint = currentPoint;
			currentPoint = pointArray[randomint];
			FVector loc = currentPoint->GetActorLocation();
			if (OwnerEnemy->aicontroller)
			{
				OwnerEnemy->aicontroller->MoveToLocation(loc);
				TargetLoc = loc;
			}
		}
		else
		{
			bool randbool = FMath::RandBool();
			if (randbool)
			{
				int randomint = FMath::RandRange(0, pointArray.Num() - 1);
				AActor* temppoint = currentPoint;
				currentPoint = pointArray[randomint];
				FVector loc = currentPoint->GetActorLocation();
				if (OwnerEnemy->aicontroller)
				{
					OwnerEnemy->aicontroller->MoveToLocation(loc);
					TargetLoc = loc;
				}
			}
			else
			{
				FVector loc = OwnerEnemy->GetActorLocation();
				if (OwnerEnemy->aicontroller)
				{
					loc += FVector(FMath::RandRange(0, 100), FMath::RandRange(0, 100), 0);
					EPathFollowingRequestResult::Type result = OwnerEnemy->aicontroller->MoveToLocation(loc);
					if (result == EPathFollowingRequestResult::Failed)
					{
						int randomint = FMath::RandRange(0, pointArray.Num() - 1);
						AActor* temppoint = currentPoint;
						currentPoint = pointArray[randomint];
						loc = currentPoint->GetActorLocation();
						if (OwnerEnemy->aicontroller)
						{
							OwnerEnemy->aicontroller->MoveToLocation(loc);
							TargetLoc = loc;
						}
					}
					else
					{
						currentPoint = nullptr;
					}
				}
			}
		}
		StateChange(ENONCOMBAT::MOVE);
	}
}

void UAC_AI_NonCombat::StateChange(ENONCOMBAT ChageState)
{
	StateBefore = State;
	State = ChageState;
	OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 200;
	switch (State)
	{
	case ENONCOMBAT::IDLE:
		if (StateBefore == ENONCOMBAT::IDLE)
		{
			if (FMath::RandBool())
			{
				OwnerEnemy->animins->Montage_Play(OwnerEnemy->animins->IdleMontage);
				OwnerEnemy->animins->Montage_JumpToSection(OwnerEnemy->animins->IdleMontage->GetSectionName(FMath::RandRange(0, OwnerEnemy->animins->IdleMontage->GetNumSections() - 1)));
				PRINT_LOG(TEXT("dodo"));
			}
		}
		break;
	case ENONCOMBAT::MOVE:
		OwnerEnemy->animins->StopAllMontages(.5);
		break;
	default:
		break;
	}
}

void UAC_AI_NonCombat::StateIdle()
{
	
}

void UAC_AI_NonCombat::StateMove()
{
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FPathFindingQuery query;
	FAIMoveRequest req;
	req.SetGoalLocation(TargetLoc);
	req.SetAcceptanceRadius(3);
	OwnerEnemy->aicontroller->BuildPathfindingQuery(req, query);
	auto r = ns->FindPathSync(query);
	if (r.Result == ENavigationQueryResult::Success)
	{
		StateChange(ENONCOMBAT::IDLE);
	}
}
