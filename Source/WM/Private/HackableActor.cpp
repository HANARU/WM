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

void AHackableActor::OnOverlapBegin(UPrimitiveComponent* selfComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//FString ObjName = otherActor->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, ObjName);
	if (otherActor->IsA(AAI_EnemyBase::StaticClass()))
	{
		Enemy = Cast<AAI_EnemyBase>(otherActor);
	}
}

