#pragma once

#include "CoreMinimal.h"
#include "HackableActor.h"
#include "HackableActor_ctOSMain.generated.h"

UCLASS()
class WM_API AHackableActor_ctOSMain : public AHackableActor
{
	GENERATED_BODY()
	
public:
	AHackableActor_ctOSMain();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* Area;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsInArea = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsLV1 = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CountAP = 0;

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Action_Interact_Single() override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* selfComp, class AActor* otherActor, UPrimitiveComponent* otherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
		void LV1TryHackCTOS();

	UFUNCTION(BlueprintImplementableEvent)
		void LV2TryHackCTOS();

	UFUNCTION(BlueprintCallable)
		void CheckAP();
};
