#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

UCLASS()
class WM_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	AInteractableObject();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void InteractCheck();

};
