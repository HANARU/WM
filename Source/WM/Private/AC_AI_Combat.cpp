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
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Chaos/Rotation.h"
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
	Owner = Cast<AAI_EnemyBase>(GetOwner());
	Owner->PawnSensing->OnSeePawn.AddDynamic(this, &UAC_AI_Combat::OnSeePawn);
	Owner->PawnSensing->OnHearNoise.AddDynamic(this, &UAC_AI_Combat::OnHearNoise);
	Owner->OnTreatDelegate.BindUObject(this, &UAC_AI_Combat::OnThreat);
}


// Called every frame
void UAC_AI_Combat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!Owner->bIsBattle) return;
	if (Owner->bIshit) return;
	if (Owner->bIsdie) return;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SitTimer = FMath::Max(0, SitTimer - DeltaTime);
	if (SitTimer == 0)
	{
		if (State == ECOMBAT::HIDDEN)
		{
			bIsSit = false;
			StateChange(ECOMBAT::ATTACK);
		}
		else
		{
			bIsSit = FMath::RandBool();
		}
		Owner->animins->bIsSit = bIsSit;
		SitTimer = FMath::RandRange(5, 10);
	}
	if (Owner && Owner->bIsBattle)
	{
		switch (State)
		{
		case ECOMBAT::ATTACK:
			StateAttack();
			break;
		case ECOMBAT::HIDDEN:
			StateCover();
			break;
		case ECOMBAT::HIDDENBEWARE:
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
	if (Owner->SeeingTimer > 0)
	{
		Owner->SeeingTimer -= DeltaTime;
		if (Owner->Target)
		{
			if (Owner->SeeingTimer <= 0 && State == ECOMBAT::ATTACK)
			{
				Owner->TargetDir = Owner->Target->GetActorLocation() - Owner->TargetLoc;
				Owner->TargetDir.Normalize();
				Owner->TargetLoc = Owner->Target->GetActorLocation();
				Owner->Target = nullptr;
				EPathFollowingRequestResult::Type result;
				result = Owner->aicontroller->MoveToLocation(Owner->TargetLoc);
				moveLoc = Owner->TargetLoc;
				int random = FMath::RandRange(0, 100);
				if (bIsSit || (result == EPathFollowingRequestResult::Failed && random > courage))
				{
					Owner->aicontroller->StopMovement();
					StateChange(ECOMBAT::HOLD);
					StateTimer = FMath::RandRange(3, 7);
				}
				else
				{
					StateChange(ECOMBAT::CHASE);
					Owner->GetCharacterMovement()->MaxWalkSpeed = 100;
				}
			}
			else
			{
				Owner->TargetDir = Owner->Target->GetActorLocation() - Owner->TargetLoc;
				Owner->TargetDir.Normalize();
				Owner->TargetLoc = Owner->Target->GetActorLocation();
				Owner->animins->aimDir = 0;
			}
		}
	}
	if (bIsFocus)
	{
		if (Owner->Target && Owner->SeeingTimer > 0)
		{
			Owner->SetActorRotation(FRotator(0, (Owner->Target->GetActorLocation() - Owner->GetActorLocation()).Rotation().Yaw, 0));
			FVector SubDir = firepoint - Owner->firepoint->GetComponentLocation();
			Owner->animins->aimDir = SubDir.Rotation().Pitch;
			Fire();
		}
		else
		{
			Owner->SetActorRotation(FRotator(0, (Owner->TargetLoc - Owner->GetActorLocation()).Rotation().Yaw, 0));
			FVector SubDir = Owner->TargetLoc - Owner->firepoint->GetComponentLocation();
			Owner->animins->aimDir = SubDir.Rotation().Pitch;
		}

	}
	else
	{
		Owner->animins->aimDir = 0;
	}
}

void UAC_AI_Combat::Fire()
{
	FireTimer = FMath::Max(0, FireTimer - GetWorld()->DeltaRealTimeSeconds);
	if (Owner->bIshit) return;
	if (Owner->firepoint && FireTimer == 0 && !TargetBones.IsEmpty())
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);
		FName bonename = TargetBones[FMath::RandRange(0, TargetBones.Num() - 1)];
		FVector boneloc = Owner->Target->GetMesh()->GetBoneLocation(bonename);
		firepoint = boneloc;
		if(Owner->Target)
		Owner->Target->MakeNoise(1., nullptr, Owner->GetActorLocation(), 1000.);
		float distance = FVector::Distance(Owner->GetActorLocation(), boneloc);
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
		boneloc = boneloc + randvec * maxrand - Owner->GetActorLocation();
		boneloc.Normalize();
		FTransform trans;
		trans.SetLocation(Owner->firepoint->GetComponentLocation());
		trans.SetRotation(FQuat::MakeFromRotator(boneloc.Rotation()));
		trans.SetScale3D(FVector(.05, .1, .05));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, trans);
		boneloc = Owner->GetActorLocation() + boneloc * 5000;
		GetWorld()->LineTraceSingleByChannel(HitResult, Owner->firepoint->GetComponentLocation(), boneloc, ECC_GameTraceChannel6, QueryParams);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), firesound, Owner->firepoint->GetComponentLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), bulletsound, firepoint, FMath::FRandRange(0., 1.));
		DrawDebugLine(GetWorld(), Owner->firepoint->GetComponentLocation(), HitResult.Location, FColor::Red, false, -1.f, 0, 2.0f);
		Owner->animins->Montage_Play(Owner->animins->fireMontage);
		if (HitResult.bBlockingHit)
		{
			AMyPlayer* player = Cast<AMyPlayer>(HitResult.GetActor());
			if (player)
			{
				count++;
				DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1);
			}
		}
		FireTimer = FireTimerMax;
	}
}

void UAC_AI_Combat::OnThreat()
{
	if (Owner->bIsdie) return;
	if (Owner->GetMesh()->GetAnimationMode() != EAnimationMode::AnimationBlueprint)
	{
		Owner->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		Owner->animins = Cast<UAI_EnemyAnimInstance>(Owner->GetMesh()->GetAnimInstance());
		if (Owner->animins)
		{
			//PRINT_LOG(TEXT("okey1"));
			if (!Owner->animins->Owner)
			{
				//PRINT_LOG(TEXT("okey2"));
				Owner->animins->Owner = Owner;
			}
		}
		//PRINT_LOG(TEXT("oo"));
		//if (Owner->animclass)
		//{
		//	//Owner->GetMesh()->SetAnimClass(Owner->animclass);
		//	//PRINT_LOG(TEXT("okey"));
		//}
	}
	Owner->animins->bIsCombat = Owner->bIsBattle;
	AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
	if (FMath::RandRange(0, 20) == 0)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), vocalsound, Owner->GetActorLocation());
	}
	Owner->animins->StopAllMontages(.5);
	Owner->Target = player;
	Owner->SeeingTimer = 1.0;
	if (State == ECOMBAT::CHASE)
		Owner->aicontroller->StopMovement();
	if (State == ECOMBAT::HIDDENBEWARE || State == ECOMBAT::HIDDENRUN) return;
	Owner->bUseControllerRotationYaw = false;
	if (!Owner->bIsBattle)
	{
		FindAndMoveCover();
		if (FMath::RandBool())
		{
			StateChange(ECOMBAT::HIDDENBEWARE);
		}
		else
		{
			StateChange(ECOMBAT::HIDDENRUN);
		}
		CoverTimer = 3;
		Owner->PawnSensing->SetPeripheralVisionAngle(90);
		player->isInCombat++;
		Owner->bIsBattle = true;
		Owner->bIsCombat = true;
	}
	else
	{
		StateChange(ECOMBAT::ATTACK);
	}
}

FHitResult UAC_AI_Combat::LineTraceSocket(FName SocketName, ACharacter* TargetCharacter)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	FHitResult Hit;
	FVector TraceStart = Owner->GetMesh()->GetSocketLocation(FName(TEXT("head")));
	FVector TraceEnd = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);
	//DrawDebugLine(GetWorld(), TraceStart, Hit.Location, FColor::Blue, false, 1);
	return Hit;
}

void UAC_AI_Combat::OnSeePawn(APawn* OtherPawn)
{
	if (Owner->bIsdie || Owner->bIshit) return;
	AMyPlayer* player = Cast<AMyPlayer>(OtherPawn);
	if (player)
	{
		int result = 0;
		FHitResult HitResult;
		HitResult = LineTraceSocket(FName("head"), player);
		TargetBones.Empty();
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 80;
			TargetBones.Add(FName("head"));
		}
		HitResult = LineTraceSocket(FName("spine_03"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 130;
			TargetBones.Add(FName("spine_03"));
		}
		HitResult = LineTraceSocket(FName("hand_r"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("hand_r"));
		}
		HitResult = LineTraceSocket(FName("hand_l"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("hand_l"));
		}
		HitResult = LineTraceSocket(FName("foot_r"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("foot_r"));
		}
		HitResult = LineTraceSocket(FName("foot_l"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("foot_l"));
		}
		PRINT_LOG(TEXT("%d"), result);
		if (result > 100 - Owner->bIsBattle * 80)
		{
			if (Owner->bIsBattle)
			{
				OnThreat();
			}
			else
			{
				Owner->SeeingTimer = 1.0;
				Owner->Target = player;
			}
		}
	}
}

void UAC_AI_Combat::OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume)
{
	if (Owner->bIsdie || Owner->bIshit) return;
	if (OtherPawn->IsA(AAI_EnemyBase::StaticClass()))
	{
		PRINT_LOG(TEXT("shotsound"));
	}
	if (Volume > .5)
	{
		PRINT_LOG(TEXT("hearsound"));
		Owner->TargetDir = Location - Owner->GetActorLocation();
		Owner->TargetDir.Normalize();
		if (!Owner->bIsBattle)
		{
			Owner->aicontroller->StopMovement();
			Owner->SetActorRotation(FRotator(0, Owner->TargetDir.Rotation().Yaw, 0));
		}
	}
}

void UAC_AI_Combat::StateAttack()
{
	CoverTimer -= GetWorld()->DeltaRealTimeSeconds;
	if (CoverTimer <= 0)
	{
		if (FMath::RandBool())
		{
			if (FindAndMoveCover())
			{
				if (FMath::RandBool())
				{
					StateChange(ECOMBAT::HIDDENBEWARE);
				}
				else
				{
					StateChange(ECOMBAT::HIDDENRUN);
				}
			}
		}
		else
		{
			StateChange(ECOMBAT::HIDDEN);
		}
		CoverTimer = FMath::RandRange(3, 7);
	}
}
void UAC_AI_Combat::StateChase()
{
	if ((Owner->TargetLoc - Owner->GetActorLocation()).Size() > 100)
	{
		FRotator rot = (Owner->TargetLoc - Owner->GetActorLocation()).Rotation();
		Owner->SetActorRotation(FRotator(0, rot.Yaw, 0));
	}
	else
	{
		FRotator rot = (Owner->TargetLoc + Owner->TargetDir * 100 - Owner->GetActorLocation()).Rotation();
		Owner->SetActorRotation(FRotator(0, rot.Yaw, 0));
		DrawDebugSphere(GetWorld(), Owner->TargetLoc, 30.f, 0, FColor::Black, false, 1);
		DrawDebugSphere(GetWorld(), Owner->TargetLoc + Owner->TargetDir * 100, 30.f, 0, FColor::Black, false, 1);
	}
	if (Owner->GetCharacterMovement()->Velocity == FVector::ZeroVector)
	{
		StateChange(ECOMBAT::HOLD);
		if (Owner->TargetDir != FVector::ZeroVector)
		{
			Owner->SetActorRotation(FRotator(0, Owner->TargetDir.Rotation().Yaw, 0));
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
		//AMyPlayer* player = Cast<AMyPlayer> (UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
		//if (player)
		//{
		//	player->isInCombat--;
		//}
		//Owner->TargetDir = FVector::ZeroVector;
		//Owner->TargetLoc = FVector::ZeroVector;
		//Owner->bIsBattle = false;
		//Owner->PawnSensing->SetPeripheralVisionAngle(60);
		//Owner->bUseControllerRotationYaw = true;
	}
}
void UAC_AI_Combat::StateMoveCover()
{
	if (Owner->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		PRINT_LOG(TEXT("CoverFinish"));
		StateChange(ECOMBAT::ATTACK);
	}
}
void UAC_AI_Combat::StateMoveCoverRun()
{
	if (Owner->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		PRINT_LOG(TEXT("RUNEnd"));
		FRotator rot = (Owner->TargetLoc - Owner->GetActorLocation()).Rotation();
		Owner->SetActorRotation(FRotator(0, rot.Yaw, 0));
		Owner->bUseControllerRotationYaw = false;
		bIsSit = true;
		SitTimer = 4;
		StateChange(ECOMBAT::ATTACK);
	}
}
void UAC_AI_Combat::StateCover()
{
	
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
		if ((Owner->GetActorLocation() - hideloc.Loc).Size() > 1000)
			getarray.RemoveAt(i);
		else
			i++;
	}
	if (getarray.IsEmpty()) return false;

	hideloc = getarray[FMath::RandRange(0, getarray.Num() - 1)];
	DrawDebugSphere(GetWorld(), hideloc.Loc, 70.f, 8, FColor::Blue, false, 5);
	EPathFollowingRequestResult::Type result;
	result = Owner->aicontroller->MoveToLocation(hideloc.Loc);
	if (result == EPathFollowingRequestResult::Failed)
		return false;

	PRINT_LOG(TEXT("Find!"));
	bIsWall = hideloc.bIsWall;

	return true;
}


void UAC_AI_Combat::StateChange(ECOMBAT ChageState)
{
	
	State = ChageState;
	Owner->GetCharacterMovement()->MaxWalkSpeed = 200;
	switch (State)
	{
	case ECOMBAT::ATTACK:
		bIsFocus = true;
		break;
	case ECOMBAT::HIDDEN:
		bIsSit = true;
		bIsFocus = true;
		SitTimer = FMath::RandRange(5, 7);
	case ECOMBAT::HIDDENBEWARE:
		Owner->GetCharacterMovement()->MaxWalkSpeed = FMath::RandRange(100, 300);
		bIsFocus = true;
		PRINT_LOG(TEXT("Cover1Start"));
		break;
	case ECOMBAT::HIDDENRUN:
		Owner->bUseControllerRotationYaw = true;
		bIsFocus = false;
		bIsSit = false;
		Owner->GetCharacterMovement()->MaxWalkSpeed = 600;
		PRINT_LOG(TEXT("Cover2Start"));
		break;
	case ECOMBAT::CHASE:
		Owner->GetCharacterMovement()->MaxWalkSpeed = 100;
		bIsFocus = true;
		break;
	case ECOMBAT::HOLD:
		bIsFocus = true;
		break;
	default:
		break;
	}
}

