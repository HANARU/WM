#include "HackableActor_Lift.h"

AHackableActor_Lift::AHackableActor_Lift()
{

	LiftCover = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cover"));
	LiftCover->SetupAttachment(BodyMesh);

	CollisionArea->SetSphereRadius(128);

}
