#include "HackableActor.h"
#include "AI_EnemyBase.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"

AHackableActor::AHackableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	CollisionArea = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionArea"));
	CollisionArea->SetupAttachment(RootComponent);
	CollisionArea->SetSphereRadius(40);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AHackableActor::BeginPlay()
{
	Super::BeginPlay();
	//CollisionArea->OnComponentBeginOverlap.AddDynamic(this, &AHackableActor::OnOverlapBegin);

}

void AHackableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

