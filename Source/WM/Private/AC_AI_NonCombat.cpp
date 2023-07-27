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
	Owner = Cast<AAI_EnemyBase>(GetOwner());
	float nearDis = 50000;
	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor)
		{
			if (Actor->ActorHasTag(FName(FString::Printf(TEXT("patrol%d"), pointtagindex))))
			{
				pointArray.Add(Actor);
				if ((Actor->GetActorLocation() - Owner->GetActorLocation()).Size() < nearDis)
				{
					nearDis = (Actor->GetActorLocation() - Owner->GetActorLocation()).Size();
					currentPoint = Actor;
				}
			}
		}
	}
}


// Called every frame
void UAC_AI_NonCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (Owner->bIsdie) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Owner && !Owner->bIsBattle)
	{
		if (patrolType)
		{
			StateTimer = FMath::Max(0, StateTimer - DeltaTime);
			if (StateTimer == 0)
			{
				StateTimer = FMath::RandRange(StateTimerMin, StateTimerMax);
				SetRandomPoint();
			}
		}
		else
		{
			if (!pointArray.IsEmpty())
			{
				if (currentPoint)
				//PRINT_LOG(TEXT("%f"), (currentPoint->GetActorLocation() - Owner->GetActorLocation()).Size());
				if (!patrolType && (Owner->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle))
				{
					int index = pointArray.Find(currentPoint);
					AActor* temppoint = currentPoint;
					index = (index + 1) % pointArray.Num();
					currentPoint = pointArray[index];
					FVector loc = currentPoint->GetActorLocation();
					if (Owner->aicontroller)
					{
						Owner->aicontroller->MoveToLocation(loc + FVector(FMath::RandRange(0, 300), FMath::RandRange(0, 300), 0));
						TargetLoc = loc;
					}
				}
			}
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
	if (pointArray.IsEmpty())
	{
		StateChange(ENONCOMBAT::IDLE);
		return;
	}
	if (FMath::RandBool())
	{
		Owner->aicontroller->StopMovement();
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
			if (Owner->aicontroller)
			{
				Owner->aicontroller->MoveToLocation(loc);
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
				if (Owner->aicontroller)
				{
					Owner->aicontroller->MoveToLocation(loc);
					TargetLoc = loc;
				}
			}
			else
			{
				FVector loc = Owner->GetActorLocation();
				if (Owner->aicontroller)
				{
					loc += FVector(FMath::RandRange(0, 100), FMath::RandRange(0, 100), 0);
					EPathFollowingRequestResult::Type result = Owner->aicontroller->MoveToLocation(loc);
					if (result == EPathFollowingRequestResult::Failed)
					{
						int randomint = FMath::RandRange(0, pointArray.Num() - 1);
						AActor* temppoint = currentPoint;
						currentPoint = pointArray[randomint];
						loc = currentPoint->GetActorLocation();
						if (Owner->aicontroller)
						{
							Owner->aicontroller->MoveToLocation(loc);
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
	Owner->GetCharacterMovement()->MaxWalkSpeed = 200;
	switch (State)
	{
	case ENONCOMBAT::IDLE:
		if (StateBefore == ENONCOMBAT::IDLE)
		{
			if (FMath::RandRange(0, 5) == 0)
			{
				Owner->animins->Montage_Play(Owner->animins->IdleMontage);
				Owner->animins->Montage_JumpToSection(Owner->animins->IdleMontage->GetSectionName(FMath::RandRange(0, Owner->animins->IdleMontage->GetNumSections() - 1)));
				PRINT_LOG(TEXT("dodo"));
			}
		}
		break;
	case ENONCOMBAT::MOVE:
		Owner->animins->StopAllMontages(.5);
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
	Owner->aicontroller->BuildPathfindingQuery(req, query);
	auto r = ns->FindPathSync(query);
	if (r.Result == ENavigationQueryResult::Success)
	{
		StateChange(ENONCOMBAT::IDLE);
	}
}
