// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_AI_RunCombat.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WM_API UAC_AI_RunCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_AI_RunCombat();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OnThreat();
public:
	UPROPERTY()
	class AAI_EnemyBase* Owner;
	UPROPERTY()
	bool bIsTerrified;
	UPROPERTY()
	FVector runDir;
	UPROPERTY(EditAnywhere, Category = "FSMBATTLE")
	class USoundCue* screamsound;
	UFUNCTION(BlueprintCallable)
	void OnSeePawn(APawn *OtherPawn);
	UFUNCTION(BlueprintCallable)
	void OnHearNoise(APawn* OtherPawn, const FVector& Location, float Volume);
};
