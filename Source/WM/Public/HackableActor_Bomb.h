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

	virtual void Action_Interact() override;
	
};
