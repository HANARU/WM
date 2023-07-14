// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_EnemyBase.h"
#include "AIModule/Classes/Perception/PawnSensingComponent.h"
#include "MyPlayer.h"
#include "AIModule/Classes/AIController.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AC_AI_NonCombat.h"
#include "AC_AI_Combat.h"
#include "../WM.h"
#include "Components/CapsuleComponent.h"
#include "AI_EnemyAnimInstance.h"
// Sets default values
AAI_EnemyBase::AAI_EnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkel(TEXT("/Script/Engine.SkeletalMesh'/Game/4_SK/SKM_Quinn.SKM_Quinn'"));
	AutoPossessAI = EAutoPossessAI::Disabled;
	idleComp = CreateDefaultSubobject<UAC_AI_NonCombat>(TEXT("ACNonCombat"));
	battComp = CreateDefaultSubobject<UAC_AI_Combat>(TEXT("ACCombat"));
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(60);
	GetCharacterMovement()->MaxWalkSpeed = 200;

	if (tempSkel.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempSkel.Object);
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	}
}

void AAI_EnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAI_EnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SmoothDir = GetActorForwardVector();
	idleComp->OwnerEnemy = this;
	battComp->OwnerEnemy = this;
	PawnSensing->OnSeePawn.AddDynamic(this, &AAI_EnemyBase::OnSeePawn);
	PawnSensing->OnHearNoise.AddDynamic(this, &AAI_EnemyBase::OnHearNoise);
	aicontroller = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), GetActorTransform());
	aicontroller->Possess(this);
	animins = Cast<UAI_EnemyAnimInstance> (GetMesh()->GetAnimInstance());
	for (UActorComponent* Component : GetComponents())
	{
		if (Component->GetName() == "Fpoint")
		{
			USceneComponent* Scene = Cast<USceneComponent>(Component);
			if (Scene)
			{
				firepoint = Scene;
			}
		}
	}
}
void AAI_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FHitResult AAI_EnemyBase::LineTraceSocket(FName SocketName, ACharacter* TargetCharacter)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult Hit;
	FVector TraceStart = GetMesh()->GetSocketLocation(FName(TEXT("head")));
	FVector TraceEnd = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);
	return Hit;
}

void AAI_EnemyBase::OnSeePawn(APawn* OtherPawn)
{
	if (bIsdie) return;
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
		HitResult = LineTraceSocket(FName("ball_r"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("ball_r"));
		}
		HitResult = LineTraceSocket(FName("ball_l"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("ball_l"));
		}
		if (result > 100 - bIsBattle * 80)
		{
			SetAttack(player);
		}
	}
}

void AAI_EnemyBase::OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume)
{
	if (bIsdie) return;
	if (Volume > .5)
	{
		TargetDir = Location - GetActorLocation();
		TargetDir.Normalize();
		if (!bIsBattle)
		{
			aicontroller->MoveToLocation(Location);
			SetActorRotation(TargetDir.Rotation());
		}
	}
}

void AAI_EnemyBase::SetAttack(AMyPlayer* player)
{
	Target = player;
	player->isInCombat = true;
	SeeingTimer = 1.0;
	if (battComp->State == ECOMBAT::HIDDEN || battComp->State == ECOMBAT::HIDDENRUN) return;
	bUseControllerRotationYaw = false;
	aicontroller->StopMovement();
	if (!bIsBattle)
	{
		battComp->FindAndMoveCover();
		if (FMath::RandBool())
		{
			battComp->StateChange(ECOMBAT::HIDDEN);
		}
		else
		{
			battComp->StateChange(ECOMBAT::HIDDENRUN);
		}
		battComp->CoverTimer = 3;
		bIsBattle = true;
	}
	else
	{
		battComp->StateChange(ECOMBAT::ATTACK);
	}
}

void AAI_EnemyBase::SetDie(AMyPlayer* player)
{
	player->isInCombat = false;
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bIsdie = true;
}