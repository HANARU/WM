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


//------------------------������ �� ���� ����---------------------//
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

//-----------------------���� �Լ� �� �ΰ� ����-------------------//
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//--------------------�̵�(Movement) �Լ� �� ����----------------//
public:
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void CoverCheck(const FInputActionValue& value);
	void Vault(const FInputActionValue& value);
	void Run(const FInputActionValue& value);

	float Speed = 300.f;
	float WalkSpeed = 300.f;
	float RunSpeed = 600.f;
	
	class UAnimMontage* ClimbMontageToPlay;
 

//--------------------��ŷ �� ��ȣ�ۿ� �Լ� �� ����----------------//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsPossessing = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float InteractionTime = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Distance = 2000;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ACCTV* CCTV;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor* HackableActor;


//-----------------------���� ���� �Լ� �� ����-------------------//
public:
	// ������ World ���� ��ġ
	FVector HitActorOrigin;
	FVector HitActorExtent;
	// ���� ���������� ���� ���� ���� ���� ��ġ
	FVector NewLocation;
	// �浹ü�� ũ��
	float DistanceToCoverObject = 200;
	// ���� �ϴ����ΰ�?
	bool isCovering = false;
	// ������ ��� Object�� Normal Vector
	FVector CoverObjectNormal;
	FVector CoverObjectOrthogonal;

	// �������ΰ�?
	bool nowCovering = false;
	bool leftDetect = false;
	bool rightDetect =false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canGoDetect = false;
	bool attached = false;


	//������ ��Ұ� �ִ��� �Ǵ��ϴ� �Լ�
	void Covering();
	bool ConverLineTrace(float degree);
	float LineTraceDegree = 15;
	FHitResult CoverLineHitResult;
	//���� �� �����ӿ� ���� ����
	void CoverMovement();
	float GetDeltaRotate();


//-----------------------Vault Warp�� ���� �Լ�, ����-------------------//

	// ���� ��ü�� ������ ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float DistanceToObject = 70;
	// ������ �� �ִ� �Ѱ� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float VaultLimit = 150;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float LineDelta = 50;

	// �پ���� �� �ִ°�?
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

//-----------------------������ ���� �Լ�, ����-------------------//

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Gun")
		bool isZooming = false;

	
	UFUNCTION(BlueprintCallable)
		void Shoot();
	UFUNCTION(BlueprintCallable)
		void ZoomIn();
	UFUNCTION(BlueprintCallable)
		void ZoomOut();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class USceneComponent* ShootStartPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShootRange = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HorizontalRecoil = -0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VerticalRecoil = -0.5f;
};
