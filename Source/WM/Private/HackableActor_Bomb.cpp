#include "HackableActor_Bomb.h"
#include "AI_EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

AHackableActor_Bomb::AHackableActor_Bomb()
{
	FXExplosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	FXExplosion->SetupAttachment(CollisionArea);
	FXExplosion->SetAutoActivate(false);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("Force"));
	RadialForceComponent->Radius = 200;
	RadialForceComponent->ImpulseStrength = 10000.f;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionAsset(TEXT("/Script/Engine.ParticleSystem'/Game/6_MISC/FX/Explosion/VFX_GrenadeEXP_default.VFX_GrenadeEXP_default'"));
	if (ExplosionAsset.Succeeded())
	{
		FXExplosion->SetTemplate(ExplosionAsset.Object);
	}
}

void AHackableActor_Bomb::Action_Interact()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Explosion"));

	FVector ExplosionLocation = GetActorLocation();

	FTimerHandle TimerHandle;
	FXExplosion->ToggleActive();

	Explosion();
	//FString ObjName = Enemy->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, ObjName);

	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			Destroy();

		}), 2.f, false);
}

void AHackableActor_Bomb::Explosion()
{
	RadialForceComponent->FireImpulse();
	float ImpulseValue = RadialForceComponent->ImpulseStrength;
	FVector ImpulseVector = FVector(ImpulseValue, ImpulseValue, ImpulseValue);
	Enemy->GetMesh()->SetSimulatePhysics(true);
	Enemy->GetMesh()->AddImpulse(ImpulseVector);
}
