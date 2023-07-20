#include "HackableActor_ctOSAP.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

void AHackableActor_ctOSAP::Tick(float DeltaTime)
{
	FXSpark = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	//FXSpark->SetupAttachment(RootComponent);
	FXSpark->SetAutoActivate(false);
	Object_Num = TEXT("2");
}

void AHackableActor_ctOSAP::Action_Interact_Single()
{
	
}
