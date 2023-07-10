#include "HackableActor.h"
#include "AI_EnemyBase.h"
#include "Components/SphereComponent.h"

AHackableActor::AHackableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionArea = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionArea"));
	RootComponent = CollisionArea;
	CollisionArea->SetSphereRadius(40);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	BodyMesh->SetupAttachment(CollisionArea);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AHackableActor::BeginPlay()
{
	Super::BeginPlay();
}

void AHackableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHackableActor::Action_Interact()
{
}

