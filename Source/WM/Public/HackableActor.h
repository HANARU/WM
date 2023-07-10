#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HackableActor.generated.h"

UCLASS()
class WM_API AHackableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHackableActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* CollisionArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class AAI_EnemyBase* Enemy;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void Action_Interact();

};
