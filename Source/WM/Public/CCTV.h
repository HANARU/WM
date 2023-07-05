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
		class AMyPlayer* Player;
//		TSubclassOf<class AMyPlayer> Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance = 20000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsUsing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ACCTV* TrackedOtherCCTV;
//		TSubclassOf<class ACCTV> TrackedOtherCCTV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPostProcessComponent* HackingTransition;

protected:
	virtual void BeginPlay() override;

	
public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ActivateCCTV();

	void PossessCCTV();

	void TrackInteractable();

	UFUNCTION(BlueprintCallable)
		void InteractStart_1Sec();
	UFUNCTION(BlueprintCallable)
		void InteractEnd_1Sec();
	UFUNCTION(BlueprintCallable)
		void InteractionSinglePress();
	UFUNCTION(BlueprintCallable)
		void Back2Player(AMyPlayer* SinglePlayer, APlayerController* PlayerController);

};
