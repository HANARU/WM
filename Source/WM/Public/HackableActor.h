#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "HackableActor.generated.h"

UCLASS()
class WM_API AHackableActor : public APawn
{
	GENERATED_BODY()
	
public:	
	AHackableActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* CollisionArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Object_Num;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNeed2Subtract = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)		// False : 0.5Sec, True : Single
		bool bIsSinglePress = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class AAI_EnemyBase* Enemy;

	float InteractionTime = 0;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Action_Interact_Single() {}

	virtual void Action_Interact_Second(float Second) {}

};
