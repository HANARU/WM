// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/CapsuleComponent.h>

// Sets default values
AMyPlayer::AMyPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Camera วาด็
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(GetCapsuleComponent());
	CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("tpsCamComp"));
	CamComp->SetupAttachment(springArmComp);


	bUseControllerRotationYaw = true;
	springArmComp->bUsePawnControlRotation = true;
	CamComp->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("ForwardAndBackward", this, &AMyPlayer::ForwardAndBackward);
	PlayerInputComponent->BindAxis("LeftAndRight", this, &AMyPlayer::LeftAndRight);
	PlayerInputComponent->BindAxis("Lookup", this, &AMyPlayer::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AMyPlayer::Turn);


}

void AMyPlayer::ForwardAndBackward(float value)
{
	if (Controller && value)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(ForwardDirection, value);
	}
}

void AMyPlayer::LeftAndRight(float value)
{
	if (Controller && value)
	{

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, value);
	}
}

void AMyPlayer::LookUp(float value)
{
	if (Controller != nullptr)
	{
		// add pitch input to controller
		AddControllerPitchInput(value);
	}
}

void AMyPlayer::Turn(float value)
{
	if (Controller != nullptr)
	{
		// add Yaw input to controller
		AddControllerYawInput(value);
	}
}

