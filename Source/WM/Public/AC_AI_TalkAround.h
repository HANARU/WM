// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_AI_TalkAround.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WM_API UAC_AI_TalkAround : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AI_TalkAround();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	UPROPERTY(EditAnywhere)
	class AAI_EnemyBase* Owner;
	UPROPERTY(EditAnywhere)
	TArray<class AAI_EnemyBase*> nearTalkerArray;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* TalkMontage;
	UPROPERTY(EditAnywhere)
	class AAI_EnemyBase* currentTalker;
	UPROPERTY(EditAnywhere)
	bool bIsTalking;
	UPROPERTY(EditAnywhere)
	float talkingTimer;
};
