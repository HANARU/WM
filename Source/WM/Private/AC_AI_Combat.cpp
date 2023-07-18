// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_Combat.h"
#include "AI_EnemyBase.h"
#include "MyPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyQuery.h"
#include "Kismet/GameplayStatics.h"
#include "../WM.h"
#include "AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "AIModule/Classes/AITypes.h"
#include "AIModule/Classes/AIController.h"
#include "AI_EnemyAnimInstance.h"
#include "AIModule/Classes/Perception/PawnSensingComponent.h"
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
	if (OwnerEnemy->bIsdie) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SitTimer = FMath::Max(0, SitTimer - DeltaTime);
	if (SitTimer == 0)
	{
		bIsSit = FMath::RandBool();
		SitTimer = FMath::RandRange(5, 10);
	}
	if (OwnerEnemy && OwnerEnemy->bIsBattle)
	{
		switch (State)
		{
		case ECOMBAT::ATTACK:
			StateAttack();
			break;
		case ECOMBAT::HIDDEN:
			StateMoveCover();
			break;
		case ECOMBAT::CHASE:
			StateChase();
			break;
		case ECOMBAT::HOLD:
			StateHold();
			break;
		case ECOMBAT::HIDDENRUN:
			StateMoveCoverRun();
			break;
		default:
			break;
		}
	}
}

void UAC_AI_Combat::Fire()
{
	if (OwnerEnemy->firepoint && FireTimer == 0 && !OwnerEnemy->TargetBones.IsEmpty())
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerEnemy);
		FName bonename = OwnerEnemy->TargetBones[FMath::RandRange(0, OwnerEnemy->TargetBones.Num() - 1)];
		FVector boneloc = OwnerEnemy->Target->GetMesh()->GetBoneLocation(bonename);
		firepoint = boneloc;
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
		boneloc = boneloc + randvec * maxrand - OwnerEnemy->GetActorLocation();
		boneloc.Normalize();
		boneloc = OwnerEnemy->GetActorLocation() + boneloc * 5000;
		//DrawDebugSphere(GetWorld(), boneloc, 50, 12, FColor::Blue, false, .1);
		GetWorld()->LineTraceSingleByChannel(HitResult, OwnerEnemy->firepoint->GetComponentLocation(), boneloc, ECC_GameTraceChannel6, QueryParams);
		DrawDebugLine(GetWorld(), OwnerEnemy->firepoint->GetComponentLocation(), boneloc, FColor::Red, false, -1.f, 0, 2.0f);
		OwnerEnemy->animins->Montage_Play(OwnerEnemy->animins->fireMontage);
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
	if (!OwnerEnemy->Target) return;
	OwnerEnemy->SeeingTimer -= GetWorld()->DeltaRealTimeSeconds;
	CoverTimer -= GetWorld()->DeltaRealTimeSeconds;
	if (CoverTimer <= 0)
	{
		if (FMath::RandBool())
		{
			if (FindAndMoveCover())
			{
				if (FMath::RandBool())
				{
					StateChange(ECOMBAT::HIDDEN);
				}
				else
				{
					StateChange(ECOMBAT::HIDDENRUN);
				}
			}
		}
		CoverTimer = FMath::RandRange(3, 7);
	}

	if (OwnerEnemy->SeeingTimer <= 0)
	{
		if (bIsWall)
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
		}
		OwnerEnemy->TargetDir = OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->TargetLoc;
		OwnerEnemy->TargetDir.Normalize();
		OwnerEnemy->TargetLoc = OwnerEnemy->Target->GetActorLocation();
		OwnerEnemy->Target = nullptr;
	}
	else
	{
		if (OwnerEnemy->SeeingTimer >= 1 - GetWorld()->DeltaRealTimeSeconds)
		{
			OwnerEnemy->TargetDir = OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->TargetLoc;
			OwnerEnemy->TargetDir.Normalize();
			OwnerEnemy->TargetLoc = OwnerEnemy->Target->GetActorLocation();
			DrawDebugSphere(GetWorld(), OwnerEnemy->TargetLoc, 30.f, 0, FColor::Black, false, 1);
		}
		OwnerEnemy->SetActorRotation(FRotator(0, (OwnerEnemy->Target->GetActorLocation() - OwnerEnemy->GetActorLocation()).Rotation().Yaw, 0));
		FireTimer = FMath::Max(0, FireTimer - GetWorld()->DeltaRealTimeSeconds);
		Fire();
	}
}
void UAC_AI_Combat::StateChase()
{
	if ((OwnerEnemy->TargetLoc - OwnerEnemy->GetActorLocation()).Size() > 100)
	{
		FRotator rot = (OwnerEnemy->TargetLoc - OwnerEnemy->GetActorLocation()).Rotation();
		OwnerEnemy->SetActorRotation(FRotator(0, rot.Yaw, 0));
	}
	else
	{
		FRotator rot = (OwnerEnemy->TargetLoc + OwnerEnemy->TargetDir * 100 - OwnerEnemy->GetActorLocation()).Rotation();
		OwnerEnemy->SetActorRotation(FRotator(0, rot.Yaw, 0));
		DrawDebugSphere(GetWorld(), OwnerEnemy->TargetLoc, 30.f, 0, FColor::Black, false, 1);
		DrawDebugSphere(GetWorld(), OwnerEnemy->TargetLoc + OwnerEnemy->TargetDir * 100, 30.f, 0, FColor::Black, false, 1);
	}
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
		OwnerEnemy->PawnSensing->SetPeripheralVisionAngle(60);
		OwnerEnemy->bUseControllerRotationYaw = true;
	}
}
void UAC_AI_Combat::StateMoveCover()
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
		if (OwnerEnemy->SeeingTimer >= 1 - GetWorld()->DeltaRealTimeSeconds || OwnerEnemy->SeeingTimer <= 0)
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
	if (OwnerEnemy->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		PRINT_LOG(TEXT("CoverFinish"));
		StateChange(ECOMBAT::ATTACK);
	}
}

void UAC_AI_Combat::StateMoveCoverRun()
{
	if (OwnerEnemy->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		PRINT_LOG(TEXT("RUNEnd"));
		FRotator rot = (OwnerEnemy->TargetLoc - OwnerEnemy->GetActorLocation()).Rotation();
		OwnerEnemy->SetActorRotation(FRotator(0, rot.Yaw, 0));
		OwnerEnemy->bUseControllerRotationYaw = false;
		bIsSit = true;
		SitTimer = 4;
		StateChange(ECOMBAT::ATTACK);
	}
}

bool UAC_AI_Combat::FindAndMoveCover()
{
	AMyQuery* getquery = Cast<AMyQuery>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyQuery::StaticClass()));
	if (!getquery) return false;
	TArray<FHideLoc> getarray = getquery->coverArray;
	if (getarray.IsEmpty()) return false;
	FHideLoc hideloc;
	int i = 0;
	while (i < getarray.Num())
	{
		hideloc = getarray[i];
		if ((OwnerEnemy->GetActorLocation() - hideloc.Loc).Size() > 1000)
			getarray.RemoveAt(i);
		else
			i++;
	}
	if (getarray.IsEmpty()) return false;

	hideloc = getarray[FMath::RandRange(0, getarray.Num() - 1)];
	DrawDebugSphere(GetWorld(), hideloc.Loc, 70.f, 6, FColor::Emerald, false, 5);
	EPathFollowingRequestResult::Type result;
	result = OwnerEnemy->aicontroller->MoveToLocation(hideloc.Loc);
	if (result == EPathFollowingRequestResult::Failed)
		return false;

	PRINT_LOG(TEXT("Find!"));
	bIsWall = hideloc.bIsWall;

	return true;
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
		OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = FMath::RandRange(100, 300);
		break;
	case ECOMBAT::HIDDENRUN:
		OwnerEnemy->bUseControllerRotationYaw = true;
		bIsSit = false;
		OwnerEnemy->GetCharacterMovement()->MaxWalkSpeed = 600;
		PRINT_LOG(TEXT("RUNStart"));
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

