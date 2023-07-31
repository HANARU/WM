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
#include "AC_AI_Hp.h"
#include <Components/AudioComponent.h>
#include "../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
// Sets default values for this component's properties
UAC_AI_Combat::UAC_AI_Combat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMaka(TEXT("/Script/Engine.StaticMesh'/Game/3_SM/Pistol/Makarov/makarov.makarov'"));
	if (tempMaka.Succeeded())
	{
		makaMesh = tempMaka.Object;
	}
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempFireEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/6_MISC/FX/BulletImpact/FX/NiagaraSystems/NS_AR_Muzzleflash_1_ONCE.NS_AR_Muzzleflash_1_ONCE'"));
	if (tempFireEffect.Succeeded())
	{
		fireEffect = tempFireEffect.Object;
	}
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempHitEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/6_MISC/FX/BulletImpact/FX/NiagaraSystems/NS_Impact_1.NS_Impact_1'"));
	if (tempHitEffect.Succeeded())
	{
		hitEffect = tempHitEffect.Object;
	}
	// ...
}


// Called when the game starts
void UAC_AI_Combat::BeginPlay()
{
	Super::BeginPlay();
	// ...
	Owner = Cast<AAI_EnemyBase>(GetOwner());

	FTransform tempTrans;
	tempTrans.SetLocation(FVector(1, -13.8, 2.65));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(90.f, -75.f, -90.f)));
	tempTrans.SetScale3D(FVector(.1, .1, .1));
	makaComp = NewObject<UStaticMeshComponent>(Owner, FName(TEXT("Makarov")));
	makaComp->RegisterComponent();
	makaComp->AttachToComponent(Owner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "hand_r");
	makaComp->SetStaticMesh(makaMesh);
	makaComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//makaComp = Owner->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("makarov"));
	//makaComp->SetupAttachment(Owner->GetMesh(), "hand_r");
	makaComp->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(.2, -84, 58));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(180.f, .5f, .5f)));
	tempTrans.SetScale3D(FVector(1, 1, 1));
	//firepointComp = Owner->CreateDefaultSubobject<USceneComponent>(TEXT("Fpoint"));
	//firepointComp->SetupAttachment(makaComp);
	//firepointComp->SetRelativeTransform(tempTrans);
	firepointComp = NewObject<USceneComponent>(Owner, FName(TEXT("Fpoint")));
	firepointComp->RegisterComponent();
	firepointComp->AttachToComponent(makaComp, FAttachmentTransformRules::KeepRelativeTransform);
	firepointComp->SetRelativeTransform(tempTrans);

	Owner->PawnSensing->OnSeePawn.AddDynamic(this, &UAC_AI_Combat::OnSeePawn);
	Owner->PawnSensing->OnHearNoise.AddDynamic(this, &UAC_AI_Combat::OnHearNoise);
	Owner->OnTreatDelegate.BindUObject(this, &UAC_AI_Combat::OnThreat);

	VocalTimer = FMath::RandRange(3, 6);
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
	VocalTimer = FMath::Max(0, VocalTimer - DeltaTime);
	if (VocalTimer == 0)
	{
		if (FMath::RandRange(0, 3) == 0)
		{
			Owner->audioComp->SetSound(vocalsound);
			Owner->audioComp->Play();
			//PRINT_LOG(TEXT("Say"));

			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), vocalsound, Owner->GetActorLocation());
		}
		VocalTimer = FMath::RandRange(3, 6);
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
			if (Owner->SeeingTimer <= 0)
			{
				Owner->TargetDir = Owner->Target->GetActorLocation() - Owner->TargetLoc;
				Owner->TargetDir.Normalize();
				Owner->TargetLoc = Owner->Target->GetActorLocation();
				Owner->Target = nullptr;
				if (State == ECOMBAT::ATTACK)
				{
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
			FVector SubDir = Owner->TargetLoc - firepointComp->GetComponentLocation();
			Owner->animins->aimDir = SubDir.Rotation().Pitch;
			Fire();
		}
		else
		{
			Owner->SetActorRotation(FRotator(0, (Owner->TargetLoc - Owner->GetActorLocation()).Rotation().Yaw, 0));
			FVector SubDir = Owner->TargetLoc - firepointComp->GetComponentLocation();
			Owner->animins->aimDir = SubDir.Rotation().Pitch;
			if (FMath::RandRange(0, 5) == 0)
			Fire();
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
	if (firepointComp && FireTimer == 0)
	{
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Owner);
		FVector boneloc;
		if (Owner->SeeingTimer > 0 && !TargetBones.IsEmpty())
		{
			FName bonename = TargetBones[FMath::RandRange(0, TargetBones.Num() - 1)];
			boneloc = Owner->Target->GetMesh()->GetBoneLocation(bonename);
			firepoint = boneloc;
		}
		else
		{
			boneloc = Owner->TargetLoc;
			firepoint = Owner->TargetLoc;

		}
		if(Owner->Target)
			Owner->Target->MakeNoise(1., nullptr, Owner->GetActorLocation(), 1000.);
		float distance = FVector::Distance(Owner->GetActorLocation(), firepoint);
		float maxrand;
		if (FMath::RandRange(0, 100) > 1)
		{
			maxrand = FMath::Min(FMath::RandRange(70, 100), distance / 2);
		}
		else
		{
			maxrand = FMath::Min(FMath::RandRange(0, 100), distance / 2);
		}
		FVector randvec = (firepoint - firepointComp->GetComponentLocation());
		FVector randvec2 = FVector(0, FMath::FRandRange(-1., 1.), FMath::FRandRange(-1., 1.));
		randvec2.Normalize();
		randvec.Normalize();
		randvec = FVector::CrossProduct(randvec, randvec2);
		//PRINT_LOG(TEXT("%f %f %f"), randvec.X, randvec.Y, randvec.Z);
		boneloc = firepoint + randvec * maxrand - Owner->GetActorLocation();
		boneloc.Normalize();
		FTransform trans;
		trans.SetLocation(firepointComp->GetComponentLocation());
		trans.SetScale3D(FVector(2));
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), fireEffect, trans);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), fireEffect, trans.GetLocation(), boneloc.Rotation(), trans.GetScale3D());
		boneloc = Owner->GetActorLocation() + boneloc * 5000;

		//DrawDebugSphere(GetWorld(), boneloc, 50, 12, FColor::Black, false, 1);
		GetWorld()->LineTraceSingleByChannel(HitResult, firepointComp->GetComponentLocation(), boneloc, ECC_GameTraceChannel6, QueryParams);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), firesound, firepointComp->GetComponentLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), bulletsound, firepoint, FMath::FRandRange(0., 1.));
		if(HitResult.bBlockingHit)
			DrawDebugLine(GetWorld(), firepointComp->GetComponentLocation(), HitResult.Location, FColor::Red, false, -1.f, 0, 2.0f);
		else
			DrawDebugLine(GetWorld(), firepointComp->GetComponentLocation(), boneloc, FColor::Red, false, -1.f, 0, 2.0f);
		Owner->animins->Montage_Play(Owner->animins->fireMontage);
		if (HitResult.bBlockingHit)
		{
			//PRINT_LOG(TEXT("%s"), *HitResult.BoneName.ToString());
			if (HitResult.BoneName.ToString() != "None")
			{
				AMyPlayer* player = Cast<AMyPlayer>(HitResult.GetActor());
				if (player)
				{
					// 플레이어 피격 모션 호출
					player->GetAttacked();
					//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50, 12, FColor::Red, false, 1);
				}
				else if(HitResult.GetActor()->IsA(AAI_EnemyBase::StaticClass()))
				{
					AAI_EnemyBase* enemy = Cast<AAI_EnemyBase>(HitResult.GetActor());
					if (enemy)
					{
						if (HitResult.BoneName == "head")
						{
							enemy->SetDie();
						}
						else
						{
							//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Enemy Hit"));

							enemy->OnThreat();
							if (enemy->hpComp)
							{
								enemy->hpComp->OnHit(4);

							}
						}
					}
				}
			}
			else
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), hitEffect, HitResult.Location, FRotator(), FVector(2));
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
	AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
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
		Owner->animins->bIsCombat = Owner->bIsBattle;
		if (FMath::RandRange(0, 3) == 0)
		{
			Owner->audioComp->SetSound(vocalsound);
			Owner->audioComp->Play();
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), vocalsound, Owner->GetActorLocation());
		}
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
		//PRINT_LOG(TEXT("CoverFinish"));
		StateChange(ECOMBAT::ATTACK);
		Owner->animins->openType = openType;
	}
}
void UAC_AI_Combat::StateMoveCoverRun()
{
	if (Owner->aicontroller->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		//PRINT_LOG(TEXT("RUNEnd"));
		FRotator rot = (Owner->TargetLoc - Owner->GetActorLocation()).Rotation();
		Owner->SetActorRotation(FRotator(0, rot.Yaw, 0));
		Owner->bUseControllerRotationYaw = false;
		bIsSit = true;
		SitTimer = 4;
		StateChange(ECOMBAT::ATTACK);
		Owner->animins->openType = openType;
	}
}
void UAC_AI_Combat::StateCover()
{
	
}
bool UAC_AI_Combat::FindAndMoveCover()
{
	openType = 0;
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
	//DrawDebugSphere(GetWorld(), hideloc.Loc, 70.f, 8, FColor::Blue, false, 5);
	EPathFollowingRequestResult::Type result;
	result = Owner->aicontroller->MoveToLocation(hideloc.Loc);
	if (result == EPathFollowingRequestResult::Failed)
		return false;

	//PRINT_LOG(TEXT("Find!"));
	bIsWall = hideloc.bIsWall;
	openType = hideloc.openType;

	return true;
}


void UAC_AI_Combat::StateChange(ECOMBAT ChageState)
{
	
	State = ChageState;
	Owner->GetCharacterMovement()->MaxWalkSpeed = 200;
	bIsHidden = false;
	Owner->animins->bIsHidden = false;
	switch (State)
	{
	case ECOMBAT::ATTACK:
		bIsFocus = true;
		break;
	case ECOMBAT::HIDDEN:
		bIsSit = true;
		bIsFocus = true;
		bIsHidden = true;
		Owner->animins->bIsHidden = true;
		SitTimer = FMath::RandRange(5, 7);
	case ECOMBAT::HIDDENBEWARE:
		Owner->GetCharacterMovement()->MaxWalkSpeed = FMath::RandRange(100, 300);
		bIsFocus = true;
		//PRINT_LOG(TEXT("Cover1Start"));
		break;
	case ECOMBAT::HIDDENRUN:
		Owner->bUseControllerRotationYaw = true;
		bIsFocus = false;
		bIsSit = false;
		Owner->GetCharacterMovement()->MaxWalkSpeed = 600;
		//PRINT_LOG(TEXT("Cover2Start"));
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

