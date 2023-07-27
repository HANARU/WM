// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_EnemyAnimInstance.h"
#include "../Public/AI_EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AC_AI_Combat.h"
#include "../WM.h"
UAI_EnemyAnimInstance::UAI_EnemyAnimInstance()
{

}

void UAI_EnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	if (::IsValid(Owner))
	{
		Velocity = Owner->GetVelocity();
		FVector XYVec = FVector(Velocity.X, Velocity.Y, 0);
		MovementSpd = XYVec.Size();
		XYVec.Normalize();
		MovementDir = CalculateDirection(XYVec, Owner->GetActorForwardVector().Rotation());
		hitTimer = Owner->HitTimer;
		aimDir = FMath::Lerp(aimDir, targetaimDir, 10 * DeltaTime);
	}
}

void UAI_EnemyAnimInstance::NativeBeginPlay()
{
	Owner = CastChecked<AAI_EnemyBase>(TryGetPawnOwner());
}
