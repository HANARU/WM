// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_TalkAround.h"
#include "AI_EnemyBase.h"
#include "EngineUtils.h"
#include "AI_EnemyAnimInstance.h"
// Sets default values for this component's properties
UAC_AI_TalkAround::UAC_AI_TalkAround()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

}


// Called when the game starts
void UAC_AI_TalkAround::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = Cast<AAI_EnemyBase>(GetOwner());
	for (TActorIterator<AAI_EnemyBase> It(GetWorld(), AAI_EnemyBase::StaticClass()); It; ++It)
	{
		AAI_EnemyBase* Enemy = *It;
		if (Enemy && Enemy != Owner)
		{
			if ((Enemy->GetActorLocation() - Owner->GetActorLocation()).Size() < 150)
			{
				if (Enemy->GetComponentByClass(UAC_AI_TalkAround::StaticClass()))
				{
					nearTalkerArray.Add(Enemy);
				}
			}
		}
	}
}


// Called every frame
void UAC_AI_TalkAround::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Owner->bIsBattle) return;
	talkingTimer = FMath::Max(0, talkingTimer - DeltaTime);
	if (talkingTimer == 0)
	{
		if (bIsTalking)
		bIsTalking = false;
		else
		bIsTalking = true;
		if (!nearTalkerArray.IsEmpty())
		{
			for (AAI_EnemyBase* Talker : nearTalkerArray)
			{
				if (Talker->GetComponentByClass(UAC_AI_TalkAround::StaticClass()))
				{
					UAC_AI_TalkAround* talkComp = Cast<UAC_AI_TalkAround>(Talker->GetComponentByClass(UAC_AI_TalkAround::StaticClass()));
					if (talkComp)
					{
						if (talkComp->bIsTalking)
						{
							bIsTalking = false;
						}
					}
				}
			}
			if (bIsTalking)
			{
				currentTalker = nearTalkerArray[FMath::RandRange(0, nearTalkerArray.Num() - 1)];
				if (TalkMontage)
				Owner->animins->Montage_Play(TalkMontage);
				Owner->animins->Montage_JumpToSection(TalkMontage->GetSectionName(FMath::RandRange(0, TalkMontage->GetNumSections() - 1)));

			}
		}
		talkingTimer = FMath::RandRange(5, 10);
	}
	// ...
}

