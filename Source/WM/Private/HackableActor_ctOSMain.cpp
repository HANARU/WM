#include "HackableActor_ctOSMain.h"
#include "Components/BoxComponent.h"

AHackableActor_ctOSMain::AHackableActor_ctOSMain()
{
	Area = CreateDefaultSubobject<UBoxComponent>(TEXT("Area"));
	Area->SetBoxExtent(FVector(128));
	Area->SetupAttachment(RootComponent);
}

void AHackableActor_ctOSMain::BeginPlay()
{
	Super::BeginPlay();
	Area->OnComponentBeginOverlap.AddDynamic(this, &AHackableActor_ctOSMain::OnOverlapBegin);
	Area->OnComponentEndOverlap.AddDynamic(this, &AHackableActor_ctOSMain::OnOverlapEnd);
}

void AHackableActor_ctOSMain::Tick(float DeltaTime)
{
	if (IsInArea)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Magenta, TEXT("Overlap on ctOS Main"));
	}
}

void AHackableActor_ctOSMain::Action_Interact_Single()
{
	if (IsInArea)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Magenta, TEXT("Interact Activated"));
	}
}

void AHackableActor_ctOSMain::OnOverlapBegin(UPrimitiveComponent* selfComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IsInArea = true;
}

void AHackableActor_ctOSMain::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsInArea = false;
}
