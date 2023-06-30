#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CCTV.generated.h"

UCLASS()
class WM_API ACCTV : public APawn
{
	GENERATED_BODY()

public:
	ACCTV();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent* CollisionArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyPlayer> Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsUsing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ACCTV> TrackedOtherCCTV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InteractionTime;


protected:
	virtual void BeginPlay() override;

	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
