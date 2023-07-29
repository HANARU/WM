// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_EnemyBase.generated.h"

DECLARE_DELEGATE(TreatDelegate)

UCLASS()
class WM_API AAI_EnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAI_EnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class UAI_EnemyAnimInstance> animclass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
	class UAC_AI_Hp* hpComp;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIsdie;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIshit;
	UPROPERTY(EditAnywhere, Category = "FSM")
	float HitTimer;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIsBattle;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIsCombat;
	//UPROPERTY(EditAnywhere, Category = "Collision")
	//TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_GameTraceChannel2;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class UPawnSensingComponent* PawnSensing;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class UAudioComponent* audioComp;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class AMyPlayer* Target;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	float SeeingTimer;


	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector TargetLoc;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector TargetDir;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector SmoothDir;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	float threatGage = 0;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class AAIController* aicontroller;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class UAI_EnemyAnimInstance* animins; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "USERAI")
	FString AI_Num;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* deathsound;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* hitsound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wrap")
	class UPawnNoiseEmitterComponent* PawnNoiseEmitter;

	TreatDelegate OnTreatDelegate;
public:
	UFUNCTION(BlueprintCallable)
	void OnThreat();
	UFUNCTION(BlueprintCallable)
	void SetDie();
};
