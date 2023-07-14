// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_AI_Hp.h"
#include "AI_EnemyBase.h"

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
	hp = maxhp;
}

void UAC_AI_Hp::OnHit(float dmg)
{
	hp -= dmg;
	if (hp <= 0 && OwnerEnemy)
	{
		OwnerEnemy->SetDie();
	}
}