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
	HIDDENBEWARE,
	HIDDENRUN,
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
	bool bIsMove;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	bool bIsFocus;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	bool bIsWall;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float FireTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float FireTimerMax = .5;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float StateTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float CoverTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float VocalTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class AAI_EnemyBase* Owner;
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
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	bool bIsSit;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	bool bIsHidden;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	int openType;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	float SitTimer;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	FVector firepoint;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* firesound;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* bulletsound;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* vocalsound;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class UParticleSystem* fireEffect;
	UPROPERTY(EditAnywhere, Category = "USERAI")
	TArray<FName> TargetBones;

	UPROPERTY(EditAnywhere, Category = "FSM")
	class UStaticMeshComponent* makaComp;
	UPROPERTY(EditAnywhere, Category = "FSM")
	class USceneComponent* firepointComp;
	UPROPERTY(EditAnywhere, Category = "FSM")
	class UStaticMesh* makaMesh;

public:
	UFUNCTION(BlueprintCallable)
	void Fire();
	UFUNCTION(BlueprintCallable)
	void OnThreat();
	UFUNCTION()
	void StateAttack();
	UFUNCTION()
	void StateChase();
	UFUNCTION()
	void StateHold();
	UFUNCTION()
	void StateChange(ECOMBAT ChageState);
	UFUNCTION()
	void StateMoveCover();
	UFUNCTION()
	void StateMoveCoverRun();
	UFUNCTION()
	void StateCover();
	UFUNCTION()
	bool FindAndMoveCover();
	UFUNCTION(BlueprintCallable)
	FHitResult LineTraceSocket(FName SocketName, ACharacter* TargetCharacter);
	UFUNCTION(BlueprintCallable)
	void OnSeePawn(APawn *OtherPawn);
	UFUNCTION(BlueprintCallable)
	void OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume);
};
