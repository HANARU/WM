// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_Combat.h"
#include "AI_EnemyBase.h"
#include "MyPlayer.h"
#include "AIModule/Classes/AIController.h"
// Sets default values for this component's properties
UAC_AI_Combat::UAC_AI_Combat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_AI_Combat::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerEnemy = Cast<AAI_EnemyBase>(GetOwner());
}


// Called every frame
void UAC_AI_Combat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (OwnerEnemy && OwnerEnemy->bIsBattle)
	{
		switch (State)
		{
		case ECOMBAT::ATTACK:
			StateAttack();
			break;
		case ECOMBAT::HIDDEN:

			break;
		case ECOMBAT::CHASE:
			StateChase();
			break;
		case ECOMBAT::HOLD:
			StateHold();
			break;
		default:
			break;
		}
	}
}

void UAC_AI_Combat::Fire()
{
	if (FireTimer == 0 && !OwnerEnemy->TargetBones.IsEmpty())
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerEnemy);
		FName bonename = OwnerEnemy->TargetBones[FMath::RandRange(0, OwnerEnemy->TargetBones.Num() - 1)];
		FVector boneloc = OwnerEnemy->Target->GetMesh()->GetBoneLocation(bonename);
		float distance = FVector::Distance(OwnerEnemy->GetActorLocation(), boneloc);
		float maxrand = FMath::Min(50, distance / 2);
		boneloc = boneloc + FVector(FMath::FRandRange(-maxrand, maxrand), FMath::FRandRange(-maxrand, maxrand), FMath::FRandRange(-maxrand, maxrand)) - OwnerEnemy->GetActorLocation();
		boneloc.Normalize();
		boneloc = OwnerEnemy->GetActorLocation() + boneloc * 5000;
		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerEnemy->GetActorLocation(), boneloc, ECC_GameTraceChannel12, QueryParams);
		DrawDebugLine(GetWorld(), OwnerEnemy->GetActorLocation(), HitResult.ImpactPoint, FColor::Red, false, -1.f, 0, 2.0f);
		if (HitResult.bBlockingHit)
		{
			AMyPlayer* player = Cast<AMyPlayer>(HitResult.GetActor());
			if (player)
			{
				//hit
			}
		}
		FireTimer = FireTimerMax;
	}
}

void UAC_AI_Combat::StateAttack()
{
	OwnerEnemy->SeeingTimer -= GetWorld()->DeltaRealTimeSeconds;
	if (OwnerEnemy->SeeingTimer <= 0)
	{
		EPathFollowingRequestResult::Type result;
		result = OwnerEnemy->aicontroller->MoveToLocation(OwnerEnemy->TargetLoc);
		moveLoc = OwnerEnemy->TargetLoc;
		int random = FMath::RandRange(0, 100);
		if (result == EPathFollowingRequestResult::Failed && random > courage)
		{
			State = ECOMBAT::HOLD;
			StateTimer = FMath::RandRange(3, 7);
		}
		else
		{
			State = ECOMBAT::CHASE;
		}
		OwnerEnemy->Target = nullptr;
	}
	else
	{
		OwnerEnemy->TargetDir = OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->GetActorLocation();
		OwnerEnemy->SetActorRotation(FRotator(0, OwnerEnemy->TargetDir.Rotation().Yaw, 0));
		FireTimer = FMath::Max(0, FireTimer - GetWorld()->DeltaRealTimeSeconds);
		Fire();
	}
}

void UAC_AI_Combat::StateChase()
{
	float dista = FVector::Distance(moveLoc, OwnerEnemy->GetActorLocation());
	if (dista < 150)
	{
		State = ECOMBAT::HOLD;
		StateTimer = FMath::RandRange(3, 7);
	}
}

void UAC_AI_Combat::StateHold()
{
	StateTimer = FMath::Max(0, StateTimer - GetWorld()->DeltaRealTimeSeconds);
	if (StateTimer == 0)
	{
		State = ECOMBAT::ATTACK;
		OwnerEnemy->TargetLoc = FVector::ZeroVector;
		OwnerEnemy->bIsBattle = false;
	}
}
