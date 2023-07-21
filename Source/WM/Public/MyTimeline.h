// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyTimeline.generated.h"

UCLASS()
class WM_API AMyTimeline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyTimeline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UTimelineComponent* MyTimeline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Timeline")
	UCurveFloat* Curve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Timeline")
	float TimelineLength;

	UFUNCTION()
	float TimelineCallback(float value);

};
