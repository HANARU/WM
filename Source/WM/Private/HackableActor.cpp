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

	CollisionArea->OnComponentBeginOverlap.AddDynamic(this, &AHackableActor::OnOverlapBegin);
	
}

void AHackableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHackableActor::Action_Interact()
{
}

void AHackableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		Enemy = Cast<AAI_EnemyBase>(OtherActor);
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Enemy Detected"));
	}
}

