// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_EnemyBase.h"
#include "AIModule/Classes/Perception/PawnSensingComponent.h"
#include "MyPlayer.h"
#include "AIModule/Classes/AIController.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AC_AI_NonCombat.h"
#include "AC_AI_Combat.h"
#include "../WM.h"
#include "Components/CapsuleComponent.h"
#include "AI_EnemyAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "AC_AI_Hp.h"
#include <Components/PawnNoiseEmitterComponent.h>
#include "Sound/SoundCue.h"
#include <Components/AudioComponent.h>
// Sets default values
AAI_EnemyBase::AAI_EnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ConstructorHelpers::FObjectFinder<USkeletalMesh> tempSkel(TEXT("/Script/Engine.SkeletalMesh'/Game/4_SK/SKM_Quinn.SKM_Quinn'"));
	AutoPossessAI = EAutoPossessAI::Disabled;

	FTransform tempTrans;

	#pragma region SetCapsuleInMesh
	//tempTrans.SetLocation(FVector(0, -9, 4));
	//tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(0.f, 90.f, 0.f)));
	//tempTrans.SetScale3D(FVector(.4, .4, .15));
	//UCapsuleComponent* capH = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_head"));
	//capH->SetupAttachment(GetMesh(), "head");
	//capH->SetRelativeTransform(tempTrans);

	//tempTrans.SetLocation(FVector(0, -3, 0));
	//tempTrans.SetRotation(FQuat::MakeFromEuler(FVector(-90.f, 0, 0.f)));
	//tempTrans.SetScale3D(FVector(.6, .6, .8));
	//UCapsuleComponent* capB = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_body"));
	//capB->SetupAttachment(GetMesh(), "spine_02");
	//capB->SetRelativeTransform(tempTrans);

	//tempTrans.SetLocation(FVector(0, -3, 0));
	//tempTrans.SetScale3D(FVector(.1, .2, .3));
	//UCapsuleComponent* capUal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_uparm_l"));
	//capUal->SetupAttachment(GetMesh(), "upperarm_l");
	//capUal->SetRelativeTransform(tempTrans);
	//UCapsuleComponent* capUar = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_uparm_r"));
	//capUar->SetupAttachment(GetMesh(), "upperarm_r");
	//capUar->SetRelativeTransform(tempTrans);

	//tempTrans.SetLocation(FVector(0, -9, 0));
	//tempTrans.SetScale3D(FVector(.1, .2, .55));
	//UCapsuleComponent* capDal = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downarm_l"));
	//capDal->SetupAttachment(GetMesh(), "lowerarm_l");
	//capDal->SetRelativeTransform(tempTrans);
	//UCapsuleComponent* capDar = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downarm_r"));
	//capDar->SetupAttachment(GetMesh(), "lowerarm_r");
	//capDar->SetRelativeTransform(tempTrans);

	//tempTrans.SetLocation(FVector(0, -3, 0));
	//tempTrans.SetScale3D(FVector(.5, .2, .55));
	//UCapsuleComponent* capUll = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_upleg_l"));
	//capUll->SetupAttachment(GetMesh(), "thigh_l");
	//capUll->SetRelativeTransform(tempTrans);
	//UCapsuleComponent* capUlr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_upleg_r"));
	//capUlr->SetupAttachment(GetMesh(), "thigh_r");
	//capUlr->SetRelativeTransform(tempTrans);

	//tempTrans.SetLocation(FVector(0, -24, 0));
	//tempTrans.SetScale3D(FVector(.25, .2, .55));
	//UCapsuleComponent* capDll = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downleg_l"));
	//capDll->SetupAttachment(GetMesh(), "calf_l");
	//capDll->SetRelativeTransform(tempTrans);
	//UCapsuleComponent* capDlr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("cap_downleg_r"));
	//capDlr->SetupAttachment(GetMesh(), "calf_r");
	//capDlr->SetRelativeTransform(tempTrans);

	//Colcapsules.Add(capB);
	//Colcapsules.Add(capH);
	//Colcapsules.Add(capDal);
	//Colcapsules.Add(capDar);
	//Colcapsules.Add(capDll);
	//Colcapsules.Add(capDlr);
	//Colcapsules.Add(capUal);
	//Colcapsules.Add(capUar);
	//Colcapsules.Add(capUll);
	//Colcapsules.Add(capUlr);
	#pragma endregion SetCapsuleInMesh

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	audioComp->SetupAttachment(GetMesh());
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(60);
	GetCharacterMovement()->MaxWalkSpeed = 200;

	hpComp = CreateDefaultSubobject<UAC_AI_Hp>(TEXT("HpComponent"));

	//if (tempSkel.Succeeded())
	//{
	//	GetMesh()->SetSkeletalMesh(tempSkel.Object);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	//}
	
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
	PawnNoiseEmitter->bAutoActivate = true;
}

void AAI_EnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAI_EnemyBase::BeginPlay()
{
	Super::BeginPlay();
	SmoothDir = GetActorForwardVector();
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
	if (!bIsBattle)
	{
		if (SeeingTimer > 0)
		{
			threatGage += DeltaTime;
			if (threatGage > 1)
			{
				OnThreat();
			}
		}
	}
	else
	{
		if (!SeeingTimer)
		{
			threatGage = FMath::Max(0, threatGage - DeltaTime*.1);
		}
	}
}

void AAI_EnemyBase::OnThreat()
{
	OnTreatDelegate.ExecuteIfBound();
}

void AAI_EnemyBase::SetDie()
{
	if (bIsdie) return;
	AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
	if (player && bIsCombat)
	{
		player->isInCombat--;
		PRINT_LOG(TEXT("%d"), player->isInCombat);
	}
	audioComp->SetSound(deathsound);
	audioComp->Play();
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), deathsound, GetActorLocation());
	GetMesh()->SetAnimClass(nullptr);
	GetMesh()->SetSimulatePhysics(true);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//for (UCapsuleComponent* capsule : Colcapsules)
	//{
	//	if(capsule)
	//	capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}
	bIsdie = true;
}