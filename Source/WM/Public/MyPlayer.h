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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* Pistol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* M416;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* CubeForAim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* SphereForAim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* CubeForAimP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	class UStaticMeshComponent* SphereForAimP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wrap")
	class UMotionWarpingComponent* MotionWraping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Wrap")
	class UPawnNoiseEmitterComponent* PawnNoiseEmitter;


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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SwitchAction;

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
	void MoveStop(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void CoverCheck(const FInputActionValue& value);
	void Vault(const FInputActionValue& value);
	void Run(const FInputActionValue& value);
	void Stop(const FInputActionValue& value);
	UFUNCTION(BlueprintCallable)
	void ChangeGun();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Move")
	void PlayStopMontage();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 HackableCount = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 HackableMaxCount = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float FillHackableCountTime = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float CurrentTime; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UUserWidget* FocusedInteractable;
	class UWidgetComponent* CCTVUi;

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

	UFUNCTION(BlueprintImplementableEvent, Category="Interact")
		void EndInteraction();
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnInteractionCCTV(FHitResult ContactActor);
	UFUNCTION(BlueprintImplementableEvent, Category = "Interact")
		void OnInteractionObject(FHitResult ContactActor);

	UFUNCTION(BlueprintCallable)
		void FillHackableCount(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor* HackableActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class AHackableActor_CCTV* HackedCCTV;

		

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	bool isCovering = false;
	// ������ ��� Object�� Normal Vector
	FVector CoverObjectNormal;
	FVector CoverObjectOrthogonal;

	// �������ΰ�?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	bool nowCovering = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
	bool leftDetect = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cover")
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
	float VaultLimit = 130;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	float LineDelta = 20;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Warp")
	FRotator StandUprototator; 

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Warp")
	FVector TempPos;

	UFUNCTION(BlueprintImplementableEvent, Category="Warp")
	void ClimbMotionWarp();
	UFUNCTION(BlueprintImplementableEvent, Category="Warp")
	void VaultMotionWarp();

//-----------------------������ ���� �Լ�, ����-------------------//

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Gun")
		bool isZooming = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Gun")
		int isInCombat = 0; 
	
	UFUNCTION()
		FVector CameraLineTrace();
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
		float ShootRange = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HorizontalRecoil = -0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VerticalRecoil = -0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gun")
	bool isRifle = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gun")
	bool isPistol = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gun")
	bool isArmed = false;

	UPROPERTY(EditDefaultsOnly, Category="Gun")
	class UParticleSystem* FireEffect;
};
