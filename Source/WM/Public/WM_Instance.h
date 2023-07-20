#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WM_Instance.generated.h"

UCLASS()
class WM_API UWM_Instance : public UGameInstance
{
	GENERATED_BODY()

public:
	UWM_Instance();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AMyPlayer* Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int HackedAPCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AAI_EnemyBase* TrackedEnemy = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor* HackableActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ACCTV* TrackedCCTV;

};
