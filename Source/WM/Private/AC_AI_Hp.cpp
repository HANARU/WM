// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_Hp.h"
#include "AI_EnemyBase.h"
#include "AI_EnemyAnimInstance.h"
#include "../WM.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "AIModule/Classes/AIController.h"

// Sets default values for this component's properties
UAC_AI_Hp::UAC_AI_Hp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_AI_Hp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<AAI_EnemyBase> (GetOwner());
	hp = maxhp;
}

void UAC_AI_Hp::OnHit(float dmg)
{
	hp -= dmg;
	if (!Owner) return;
	if (hp <= 0)
	{
		Owner->SetDie();
	}
	else
	{
		Owner->aicontroller->StopMovement();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Owner->hitsound, Owner->GetActorLocation());
		Owner->bIshit = true;
		Owner->HitTimer = FMath::FRandRange(1.4, 2.f);
		Owner->animins->Montage_Play(Owner->animins->HitMontage);
		Owner->animins->Montage_JumpToSection(Owner->animins->HitMontage->GetSectionName(FMath::RandRange(0, Owner->animins->HitMontage->GetNumSections() - 1)));
	}
}