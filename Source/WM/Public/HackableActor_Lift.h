#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_Lift.generated.h"

UCLASS()
class WM_API AHackableActor_Lift : public AHackableActor
{
	GENERATED_BODY()

public:
	AHackableActor_Lift();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* LiftCover;

public:

	
};
