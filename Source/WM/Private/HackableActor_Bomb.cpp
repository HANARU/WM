#include "HackableActor_Bomb.h"
#include "AI_EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

AHackableActor_Bomb::AHackableActor_Bomb()
{
	FXExplosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	FXExplosion->SetupAttachment(CollisionArea);
	FXExplosion->SetAutoActivate(false);

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
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			Destroy();

		}), 2.f, false);
}
