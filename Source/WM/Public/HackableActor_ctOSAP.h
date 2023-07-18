#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_ctOSAP.generated.h"

UCLASS()
class WM_API AHackableActor_ctOSAP : public AHackableActor
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
};
