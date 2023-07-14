// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_EnemyBase.generated.h"

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

	UPROPERTY(EditAnywhere, Category = "FSM")
	class UAC_AI_NonCombat* idleComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
	class UAC_AI_Combat* battComp;
	UPROPERTY(EditAnywhere, Category = "FSM")
	class USceneComponent* firepoint;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIsdie;
	UPROPERTY(EditAnywhere, Category = "FSM")
	bool bIsBattle;
	//UPROPERTY(EditAnywhere, Category = "Collision")
	//TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_GameTraceChannel2;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class UPawnSensingComponent* PawnSensing;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class AMyPlayer* Target;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	float SeeingTimer;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	TArray<FName> TargetBones;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector TargetLoc;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector TargetDir;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	FVector SmoothDir;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class AAIController* aicontroller;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	class UAI_EnemyAnimInstance* animins; 
public:
	UFUNCTION(BlueprintCallable)
	void OnSeePawn(APawn *OtherPawn);
	UFUNCTION(BlueprintCallable)
	void OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume);
	UFUNCTION(BlueprintCallable)
	void SetAttack(AMyPlayer* player);
	UFUNCTION(BlueprintCallable)
	void SetDie(AMyPlayer* player);
	UFUNCTION(BlueprintCallable)
	FHitResult LineTraceSocket(FName SocketName, ACharacter* TargetCharacter);
};
