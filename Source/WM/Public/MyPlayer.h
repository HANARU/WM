#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include <../Plugins/Animation/MotionWarping/Source/MotionWarping/Public/MotionWarpingComponent.h>
#include "MyPlayer.generated.h"

UCLASS()
class WM_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()


//------------------------생성자 및 변수 선언---------------------//
public:
	AMyPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* PlayerCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arrow)
	class UArrowComponent* CenterArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arrow)
	class UArrowComponent* Left45DetectArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Arrow)
	class UArrowComponent* Right45DetectArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wrap")
	class UMotionWarpingComponent* MotionWraping;
	
//-----------------------Enhanced Input System--------------------//
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CoverAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* VaultAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* RunAction;
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MouseLeftClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MouseRightClick;*/

//-----------------------실행 함수 및 부가 선언-------------------//
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//--------------------이동(Movement) 함수 및 변수----------------//
public:
	void Move(const FInputActionValue& value);
	void MoveStop(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void CoverCheck(const FInputActionValue& value);
	void Vault(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	
	UFUNCTION(BlueprintImplementableEvent, Category="Move")
	void PlayStopMontage();

	float Speed = 300.f;
	float WalkSpeed = 300.f;
	float RunSpeed = 600.f;
	
	class UAnimMontage* ClimbMontageToPlay;
 

//--------------------해킹 등 상호작용 함수 및 변수----------------//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsPossessing = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float InteractionTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Distance = 2000;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ACCTV* CCTV;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 HackableCount = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 HackableMaxCount = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float FillHackableCountTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float CurrentTime; 


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPostProcessComponent* HackingTransition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bProcessEvent = false;

	UFUNCTION(BlueprintCallable)
		float InteractStart_1Sec();

	UFUNCTION(BlueprintCallable)
		void InteractEnd_1Sec();

	UFUNCTION(BlueprintCallable)
		void InteractionSinglePress();

	UFUNCTION(BlueprintCallable)
		void TrackInteractable();

	UFUNCTION(BlueprintCallable)
		void FillHackableCount(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor* HackableActor;

		

//-----------------------은폐 관련 함수 및 변수-------------------//
public:
	// 엄폐물의 World 상의 위치
	FVector HitActorOrigin;
	FVector HitActorExtent;
	// 벡터 내분점으로 통해 얻은 엄폐 실제 위치
	FVector NewLocation;
	// 충돌체의 크기
	float DistanceToCoverObject = 200;
	// 엄폐를 하는중인가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	bool isCovering = false;
	// 엄폐중 얻는 Object의 Normal Vector
	FVector CoverObjectNormal;
	FVector CoverObjectOrthogonal;

	// 엄폐중인가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	bool nowCovering = false;
	bool leftDetect = false;
	bool rightDetect =false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canGoDetect = false;
	bool attached = false;


	//엄폐할 장소가 있는지 판단하는 함수
	void Covering();
	bool ConverLineTrace(float degree);
	float LineTraceDegree = 15;
	FHitResult CoverLineHitResult;
	//엄폐 후 움직임에 대한 정의
	void CoverMovement();
	float GetDeltaRotate();


//-----------------------Vault Warp를 위한 함수, 변수-------------------//

	// 넘을 물체와 나와의 간격
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float DistanceToObject = 70;
	// 점프할 수 있는 한계 높이
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float VaultLimit = 150;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float LineDelta = 50;

	// 뛰어넘을 수 있는가?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	bool canVault = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	bool canClimb = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	FVector WallPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	FVector StartPos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	FVector MiddlePos;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	FVector LastPos;

	UFUNCTION(BlueprintImplementableEvent, Category="Warp")
	void VaultMotionWarp();

//-----------------------교전을 위한 함수, 변수-------------------//

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Gun")
		bool isZooming = false;

	
	UFUNCTION(BlueprintCallable)
		void Shoot();
	UFUNCTION(BlueprintCallable)
		void ZoomIn();
	UFUNCTION(BlueprintCallable)
		void ZoomOut();
	UFUNCTION(BlueprintImplementableEvent, Category = "Gun")
		void PlayShootMontage();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USceneComponent* ShootStartPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShootRange = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HorizontalRecoil = -0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VerticalRecoil = -0.5f;
};
