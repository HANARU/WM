// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_Combat.h"
#include "AI_EnemyBase.h"
#include "MyPlayer.h"
#include "AIModule/Classes/AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		float maxrand;
		if (FMath::RandRange(0, 100) > 5)
		{
			maxrand = FMath::Min(FMath::RandRange(70, 100), distance / 2);
		}
		else
		{
			maxrand = FMath::Min(FMath::RandRange(0, 100), distance / 2);
		}
		FVector randvec = FVector(FMath::FRandRange(-1., 1.), FMath::FRandRange(-1., 1.), FMath::FRandRange(-1., 1.));
		randvec.Normalize();
		boneloc = boneloc + randvec*maxrand - OwnerEnemy->GetActorLocation();
		boneloc.Normalize();
		boneloc = OwnerEnemy->GetActorLocation() + boneloc * 5000;
		//DrawDebugSphere(GetWorld(), boneloc, 50, 12, FColor::Blue, false, .1);
		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerEnemy->GetActorLocation(), boneloc, ECC_GameTraceChannel4, QueryParams);
		DrawDebugLine(GetWorld(), OwnerEnemy->GetActorLocation(), boneloc, FColor::Red, false, -1.f, 0, 2.0f);
		if (HitResult.bBlockingHit)
		{
			AMyPlayer* player = Cast<AMyPlayer>(HitResult.GetActor());
			if (player)
			{
				count++;	
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1);
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
			StateChange(ECOMBAT::HOLD);
			StateTimer = FMath::RandRange(3, 7);
		}
		else
		{
			StateChange(ECOMBAT::CHASE);
			OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 100;
		}
		OwnerEnemy->Target = nullptr;
	}
	else
	{
		if(OwnerEnemy->SeeingTimer >= 1 - GetWorld()->DeltaRealTimeSeconds || OwnerEnemy->SeeingTimer <= 0)
		{
			if (OwnerEnemy->TargetLoc != FVector::ZeroVector)
			{
				OwnerEnemy->TargetDir = OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->TargetLoc;
				OwnerEnemy->TargetDir.Normalize();
			}
			OwnerEnemy->TargetLoc = OwnerEnemy->Target->GetActorLocation();
		}
		OwnerEnemy->SetActorRotation(FRotator(0, (OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->GetActorLocation()).Rotation().Yaw, 0));
		FireTimer = FMath::Max(0, FireTimer - GetWorld()->DeltaRealTimeSeconds);
		Fire();
	}
}

void UAC_AI_Combat::StateChase()
{
	if (OwnerEnemy->GetCharacterMovement()->Velocity == FVector::ZeroVector)
	{
		StateChange(ECOMBAT::HOLD);
		if (OwnerEnemy->TargetDir != FVector::ZeroVector)
		{
			OwnerEnemy->SetActorRotation(FRotator(0, OwnerEnemy->TargetDir.Rotation().Yaw, 0));
		}
		StateTimer = FMath::RandRange(3, 7);
	}
}

void UAC_AI_Combat::StateHold()
{
	StateTimer = FMath::Max(0, StateTimer - GetWorld()->DeltaRealTimeSeconds);
	if (StateTimer == 0)
	{
		StateChange(ECOMBAT::ATTACK);
		OwnerEnemy->TargetDir = FVector::ZeroVector;
		OwnerEnemy->TargetLoc = FVector::ZeroVector;
		OwnerEnemy->bIsBattle = false;
	}
}

void UAC_AI_Combat::StateChange(ECOMBAT ChageState)
{
	State = ChageState;
	OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 200;
	switch (State)
	{
	case ECOMBAT::ATTACK:
		break;
	case ECOMBAT::HIDDEN:
		break;
	case ECOMBAT::CHASE:
		OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 100;
		break;
	case ECOMBAT::HOLD:
		break;
	default:
		break;
	}
}
