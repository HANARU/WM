// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AI_EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class WM_API UAI_EnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public :
	UAI_EnemyAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeBeginPlay() override;
public :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	float MovementDir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	float MovementSpd;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	FVector Velocity;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	bool bIsSky;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	bool bIsCombat;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	bool bIsSit;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	float aimDir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	float targetaimDir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = OwnerValue)
	class AAI_EnemyBase* Owner;};
