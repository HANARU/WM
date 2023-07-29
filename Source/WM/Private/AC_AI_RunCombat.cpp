// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_RunCombat.h"
#include "AI_EnemyBase.h"
#include "MyPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <AIModule/Classes/AIController.h>
#include "AI_EnemyAnimInstance.h"
#include "Sound/SoundCue.h"
#include <Components/AudioComponent.h>
// Sets default values for this component's properties
UAC_AI_RunCombat::UAC_AI_RunCombat()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_AI_RunCombat::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<AAI_EnemyBase>(GetOwner());
	if (FMath::RandRange(0, 2) == 0)
	{
		PRINT_LOG(TEXT("Phone"));
		UAI_EnemyAnimInstance* animins = Cast<UAI_EnemyAnimInstance>(Owner->GetMesh()->GetAnimInstance());
		if (animins)
		animins->bIsPhone = true;
	}
	Owner->PawnSensing->OnSeePawn.AddDynamic(this, &UAC_AI_RunCombat::OnSeePawn);
	Owner->PawnSensing->OnHearNoise.AddDynamic(this, &UAC_AI_RunCombat::OnHearNoise);
	Owner->OnTreatDelegate.BindUObject(this, &UAC_AI_RunCombat::OnThreat);
}


// Called every frame
void UAC_AI_RunCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	// ...
	if (runDir == FVector::ZeroVector) return;
	if (Owner->GetVelocity() == FVector::ZeroVector)
	{
		Owner->animins->bIsTerrified = true;
	}
	else
	{
		Owner->animins->bIsTerrified = false;
	}
		Owner->GetMovementComponent()->AddInputVector(runDir);
		Owner->SetActorRotation(FRotator(0, runDir.Rotation().Yaw, 0));
}

void UAC_AI_RunCombat::OnSeePawn(APawn* OtherPawn)
{
	AMyPlayer* player = Cast<AMyPlayer> (OtherPawn);
	if (player)
	{
		if (player->isZooming)
		{
			OnThreat();
		}
	}
}

void UAC_AI_RunCombat::OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume)
{
	if (Owner->bIsdie || Owner->bIshit) return;
	OnThreat();
}

void UAC_AI_RunCombat::OnThreat()
{
	if (Owner->bIsdie) return;
	if (bIsTerrified && FMath::RandRange(0, 40) != 0) return;
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
		Owner->animins->bIsPhone = false;
		//PRINT_LOG(TEXT("oo"));
		//if (Owner->animclass)
		//{
		//	//Owner->GetMesh()->SetAnimClass(Owner->animclass);
		//	//PRINT_LOG(TEXT("okey"));
		//}
	}
	Owner->aicontroller->StopMovement();
	if (FMath::RandBool())
	{
		Owner->animins->bIsTerrified = true;
		bIsTerrified = true;
	}
	else
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = 500;
		AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
		runDir = Owner->GetActorLocation() - player->GetActorLocation();
		runDir.Normalize();
		if (!Owner->bIsBattle)
		{
			Owner->audioComp->SetSound(screamsound);
			Owner->audioComp->Play();
			//UGameplayStatics::PlaySoundAtLocation(GetWorld(), screamsound, Owner->GetActorLocation());
		}
	}
	Owner->bIsBattle = true;
}

