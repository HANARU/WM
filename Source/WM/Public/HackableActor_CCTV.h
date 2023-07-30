#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_CCTV.generated.h"

UCLASS()
class WM_API AHackableActor_CCTV : public AHackableActor
{
	GENERATED_BODY()

public:
	AHackableActor_CCTV();
	
//////////////////Visual Components//////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* CameraSupport;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* CameraBody;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyPlayer* Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Distance = 20000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsUsing;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InteractionTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPostProcessComponent* HackingTransition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UWidgetComponent* InteractableWidget;

//////////////////Trakable Variables//////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* HitActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor* HackableActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AAI_EnemyBase* TrackedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor_CCTV* TrackedOtherCCTV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsTrackingAI = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsTrackingObject = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsTrackingCCTV = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TSubclassOf<UUserWidget> FocusedInteractable;
		//UUserWidget* FocusedInteractable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OtherObjectUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* OtherCCTVUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USoundBase> Player2CCTV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TObjectPtr<USoundBase> CCTV2Player;


protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



public:
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void EndInteraction();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnInteractionCCTV(FHitResult ContactActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnInteractionObject(FHitResult ContactActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnInteractionAI(FHitResult ContactActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnTargetHacked();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnDisableProcess();
};
