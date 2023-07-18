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
#include "Kismet/GameplayStatics.h"
#include "AC_AI_Hp.h"
// Sets default values
AAI_EnemyBase::AAI_EnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMaka(TEXT("/Script/Engine.StaticMesh'/Game/3_SM/Pistol/Makarov/makarov.makarov'"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkel(TEXT("/Script/Engine.SkeletalMesh'/Game/4_SK/SKM_Quinn.SKM_Quinn'"));
	AutoPossessAI = EAutoPossessAI::Disabled;
	idleComp = CreateDefaultSubobject<UAC_AI_NonCombat>(TEXT("ACNonCombat"));
	battComp = CreateDefaultSubobject<UAC_AI_Combat>(TEXT("ACCombat"));
	hpComp = CreateDefaultSubobject<UAC_AI_Hp>(TEXT("ACHp"));

	FTransform tempTrans;

	#pragma region SetCapsuleInMesh
	tempTrans.SetLocation(FVector(0, -9, 4));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 90.f, 0.f)));
	tempTrans.SetScale3D(FVector(.4, .4, .15));
	UCapsuleComponent* capH = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_head"));
	capH->SetupAttachment(GetMesh(), "head");
	capH->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(0, -3, 0));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(-90.f, 0, 0.f)));
	tempTrans.SetScale3D(FVector(.6, .6, .8));
	UCapsuleComponent* capB = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_body"));
	capB->SetupAttachment(GetMesh(), "spine_02");
	capB->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(0, -3, 0));
	tempTrans.SetScale3D(FVector(.1, .2, .3));
	UCapsuleComponent* capUal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_uparm_l"));
	capUal->SetupAttachment(GetMesh(), "upperarm_l");
	capUal->SetRelativeTransform(tempTrans);
	UCapsuleComponent* capUar = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_uparm_r"));
	capUar->SetupAttachment(GetMesh(), "upperarm_r");
	capUar->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(0, -9, 0));
	tempTrans.SetScale3D(FVector(.1, .2, .55));
	UCapsuleComponent* capDal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downarm_l"));
	capDal->SetupAttachment(GetMesh(), "lowerarm_l");
	capDal->SetRelativeTransform(tempTrans);
	UCapsuleComponent* capDar = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downarm_r"));
	capDar->SetupAttachment(GetMesh(), "lowerarm_r");
	capDar->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(0, -3, 0));
	tempTrans.SetScale3D(FVector(.5, .2, .55));
	UCapsuleComponent* capUll = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_upleg_l"));
	capUll->SetupAttachment(GetMesh(), "thigh_l");
	capUll->SetRelativeTransform(tempTrans);
	UCapsuleComponent* capUlr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_upleg_r"));
	capUlr->SetupAttachment(GetMesh(), "thigh_r");
	capUlr->SetRelativeTransform(tempTrans);

	tempTrans.SetLocation(FVector(0, -24, 0));
	tempTrans.SetScale3D(FVector(.25, .2, .55));
	UCapsuleComponent* capDll = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downleg_l"));
	capDll->SetupAttachment(GetMesh(), "calf_l");
	capDll->SetRelativeTransform(tempTrans);
	UCapsuleComponent* capDlr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downleg_r"));
	capDlr->SetupAttachment(GetMesh(), "calf_r");
	capDlr->SetRelativeTransform(tempTrans);

	Colcapsules.Add(capB);
	Colcapsules.Add(capH);
	Colcapsules.Add(capDal);
	Colcapsules.Add(capDar);
	Colcapsules.Add(capDll);
	Colcapsules.Add(capDlr);
	Colcapsules.Add(capUal);
	Colcapsules.Add(capUar);
	Colcapsules.Add(capUll);
	Colcapsules.Add(capUlr);
	#pragma endregion SetCapsuleInMesh

	tempTrans.SetLocation(FVector(1, -13.8, 2.65));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(90.f, -75.f, -90.f)));
	tempTrans.SetScale3D(FVector(.1, .1, .1));
	makaComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("makarov"));
	makaComp->SetupAttachment(GetMesh(), "hand_r");
	makaComp->SetRelativeTransform(tempTrans);
	if (tempMaka.Succeeded())
	{
		makaComp->SetStaticMesh(tempMaka.Object);
	}

	tempTrans.SetLocation(FVector(.2, -84, 58));
	tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(180.f, .5f, .5f)));
	tempTrans.SetScale3D(FVector(1, 1, 1));
	firepoint = CreateDefaultSubobject<USceneComponent>(TEXT("Fpoint"));
	firepoint->SetupAttachment(makaComp);
	firepoint->SetRelativeTransform(tempTrans);

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
	hpComp->OwnerEnemy = this;
	PawnSensing->OnSeePawn.AddDynamic(this, &AAI_EnemyBase::OnSeePawn);
	PawnSensing->OnHearNoise.AddDynamic(this, &AAI_EnemyBase::OnHearNoise);
	aicontroller = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), GetActorTransform());
	aicontroller->Possess(this);
	animins = Cast<UAI_EnemyAnimInstance> (GetMesh()->GetAnimInstance());
	//for (UActorComponent* Component : GetComponents())
	//{
	//	if (Component->GetName() == "Fpoint")
	//	{
	//		USceneComponent* Scene = Cast<USceneComponent>(Component);
	//		if (Scene)
	//		{
	//			firepoint = Scene;
	//		}
	//	}
	//}
}
void AAI_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIshit)
	{
		HitTimer -= DeltaTime;
		if (HitTimer < 0)
		{
			bIshit = false;
		}
	}
}

FHitResult AAI_EnemyBase::LineTraceSocket(FName SocketName, ACharacter* TargetCharacter)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult Hit;
	FVector TraceStart = GetMesh()->GetSocketLocation(FName(TEXT("head")));
	FVector TraceEnd = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_GameTraceChannel4, QueryParams);
	//DrawDebugLine(GetWorld(), TraceStart, Hit.Location, FColor::Blue, false, 1);
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
		HitResult = LineTraceSocket(FName("Spine2"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 130;
			TargetBones.Add(FName("Spine2"));
		}
		HitResult = LineTraceSocket(FName("RightHand"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("RightHand"));
		}
		HitResult = LineTraceSocket(FName("LeftHand"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("LeftHand"));
		}
		HitResult = LineTraceSocket(FName("RightFoot"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("RightFoot"));
		}
		HitResult = LineTraceSocket(FName("LeftFoot"), player);
		if (HitResult.GetActor() == player)
		{
			result += HitResult.bBlockingHit * 30;
			TargetBones.Add(FName("LeftFoot"));
		}
		PRINT_LOG(TEXT("%d"), result);
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
		PRINT_LOG(TEXT("hearsound"));
		TargetDir = Location - GetActorLocation();
		TargetDir.Normalize();
		if (!bIsBattle)
		{
			aicontroller->StopMovement();
			SetActorRotation(FRotator(0, TargetDir.Rotation().Yaw, 0));
		}
	}
}

void AAI_EnemyBase::SetAttack(AMyPlayer* player)
{
	animins->StopAllMontages(.5);
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
		PawnSensing->SetPeripheralVisionAngle(90);
		bIsBattle = true;
	}
	else
	{
		battComp->StateChange(ECOMBAT::ATTACK);
	}
}

void AAI_EnemyBase::SetDie()
{
	AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
	if (player)
	{
		player->isInCombat = false;
	}
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	for (UCapsuleComponent* capsule : Colcapsules)
	{
		if(capsule)
		capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	bIsdie = true;
}