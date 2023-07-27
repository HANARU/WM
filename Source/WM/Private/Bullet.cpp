// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/BoxComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = Box;

	Tracer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Tracer"));
	Tracer->SetupAttachment(Box);

	PMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("PMovement"));


}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentTime+=DeltaTime;
	if (CurrentTime > 1)
	{
		if(this) this->Destroy();
	}
}

