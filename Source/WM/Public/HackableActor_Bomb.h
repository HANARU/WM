#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_Bomb.generated.h"

UCLASS()
class WM_API AHackableActor_Bomb : public AHackableActor
{
	GENERATED_BODY()

public:
	AHackableActor_Bomb();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* FXExplosion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class URadialForceComponent* RadialForceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* ExplosionAffectArea;

	virtual void Action_Interact() override;

	void Explosion();

	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
