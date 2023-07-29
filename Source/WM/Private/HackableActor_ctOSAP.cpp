#include "HackableActor_ctOSAP.h"
#include "HackableActor_ctOSMain.h"
#include "WM_Instance.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AHackableActor_ctOSAP::AHackableActor_ctOSAP()
{
	FXSpark = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	FXSpark->SetupAttachment(BodyMesh);
}

void AHackableActor_ctOSAP::BeginPlay()
{
}

void AHackableActor_ctOSAP::Action_Interact_Single()
{
	OnTargeted();
}
