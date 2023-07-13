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

	ExplosionAffectArea = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionArea"));
	ExplosionAffectArea->SetupAttachment(CollisionArea);
	ExplosionAffectArea->SetSphereRadius(128);

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
	

	FVector ExplosionLocation = GetActorLocation();

	FTimerHandle TimerHandle;
	FXExplosion->ToggleActive();

	Explosion();
	//FString ObjName = Enemy->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, ObjName);

	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()->void
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Explosion"));
			Destroy();
		}), 2.f, false);
	
}

void AHackableActor_Bomb::Explosion()
{
	RadialForceComponent->FireImpulse();
	float ImpulseValue = RadialForceComponent->ImpulseStrength;
	FVector ImpulseVector = FVector(ImpulseValue, ImpulseValue, ImpulseValue);
	if (IsValid(Enemy))
	{
		Enemy->GetMesh()->SetSimulatePhysics(true);
		Enemy->GetMesh()->AddImpulse(ImpulseVector);
	}
}

void AHackableActor_Bomb::BeginPlay()
{
	ExplosionAffectArea->OnComponentBeginOverlap.AddDynamic(this, &AHackableActor_Bomb::OnOverlapBegin);
}

void AHackableActor_Bomb::OnOverlapBegin(UPrimitiveComponent* selfComp, AActor* otherActor, UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString ObjName = otherActor->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, ObjName);
	if (otherActor->IsA(AAI_EnemyBase::StaticClass()))
	{
		Enemy = Cast<AAI_EnemyBase>(otherActor);
	}
	else
	{
		return;
	}
}
