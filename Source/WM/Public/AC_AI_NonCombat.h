// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_AI_NonCombat.generated.h"

UENUM(BlueprintType)
enum class ENONCOMBAT : uint8
{
	IDLE,
	MOVE,

};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WM_API UAC_AI_NonCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AI_NonCombat();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	int pointtagindex;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	TArray<AActor*> pointArray;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	AActor* currentPoint;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	FVector	TargetLoc;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	ENONCOMBAT State = ENONCOMBAT::IDLE;
public:	
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	float StateTimer;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	ENONCOMBAT StateBefore;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	class AAI_EnemyBase* OwnerEnemy;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	float StateTimerMin = 3;
	UPROPERTY(EditAnywhere, Category = "FSMIDLE")
	float StateTimerMax = 7;
public:
	UFUNCTION(BlueprintCallable)
	void SetRandomPoint();
	UFUNCTION(BlueprintCallable)
	void StateChange(ENONCOMBAT ChageState);
	UFUNCTION(BlueprintCallable)
	void StateIdle();
	UFUNCTION(BlueprintCallable)
	void StateMove();
};