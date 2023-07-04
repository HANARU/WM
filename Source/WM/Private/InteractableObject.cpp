#include "InteractableObject.h"

AInteractableObject::AInteractableObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableObject::InteractCheck()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Black, TEXT("Interact From Object"));
	Destroy();
}
