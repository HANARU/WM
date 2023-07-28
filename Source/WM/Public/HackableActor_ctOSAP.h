#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_ctOSAP.generated.h"

UCLASS()
class WM_API AHackableActor_ctOSAP : public AHackableActor
{
	GENERATED_BODY()

public:
	AHackableActor_ctOSAP();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent* FXSpark;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor_ctOSMain* ctOSMain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWM_Instance* WM;
	
public:
	virtual void BeginPlay() override;
	
	virtual void Action_Interact_Single() override;
};
