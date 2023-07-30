// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyQuery.generated.h"

USTRUCT(BlueprintType)
struct FHideLoc
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* owner = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Loc = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWall = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int openType = 0;
};

UCLASS()
class WM_API AMyQuery : public AActor
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	AMyQuery();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UPROPERTY(EditAnywhere, Category = "Query")
	bool bIsFollow = true;
	UPROPERTY(EditAnywhere, Category = "Query")
	int width = 40;
	UPROPERTY(EditAnywhere, Category = "Query")
	int height = 40;
	UPROPERTY(EditAnywhere, Category = "Query")
	float repeatTimer = 0;
	UPROPERTY(EditAnywhere, Category = "Query")
	TArray<FHideLoc> coverArray;
	UPROPERTY(EditAnywhere, Category = "Query")
	class ANavigationData* naviData;
	UPROPERTY(EditAnywhere, Category = "Query")
	class UNavigationSystemV1* naviSystem;
};
