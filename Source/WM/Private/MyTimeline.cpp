// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTimeline.h"
#include <Components/TimelineComponent.h>

// Sets default values
AMyTimeline::AMyTimeline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Curve = nullptr;

	MyTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MyTimeline"));

	TimelineLength = 0.1f;
}

// Called when the game starts or when spawned
void AMyTimeline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyTimeline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AMyTimeline::TimelineCallback(float value)
{
	// This function will be called during the timeline update.
   // 'Value' represents the current value of the curve at the current point in the timeline.
   // You can use this value to update properties or perform actions based on the animation progress.
	return 0.1f;
}

