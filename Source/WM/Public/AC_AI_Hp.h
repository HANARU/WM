// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_AI_Hp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WM_API UAC_AI_Hp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AI_Hp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:

	void OnHit(float dmg);
public:
	// Called every frame
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hp)
	class AAI_EnemyBase* OwnerEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hp)
	float maxhp = 30;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hp)
	float hp;
};
