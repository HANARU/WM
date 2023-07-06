// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_AI_Combat.generated.h"

UENUM(BlueprintType)
enum class ECOMBAT : uint8
{
	ATTACK,
	HIDDEN,
	CHASE,
	HOLD
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WM_API UAC_AI_Combat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AI_Combat();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float FireTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float FireTimerMax = .5;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float StateTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class AAI_EnemyBase* OwnerEnemy;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	ECOMBAT State = ECOMBAT::ATTACK;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float StateTimerMin = 3;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float StateTimerMax = 7;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	int courage = 40;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	FVector moveLoc;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	int count = 0;
private:
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION()
	void StateAttack();
	UFUNCTION()
	void StateChase();
	UFUNCTION()
	void StateHold();
};
