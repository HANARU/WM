#include "MyPlayer.h"
#include "Math/Vector.h"
#include "CCTV.h"
#include "AI_EnemyBase.h"
#include "HackableActor.h"
#include "InteractableObject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/CapsuleComponent.h>
#include "Components/PostProcessComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Components/ArrowComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "Math/Quat.h"
#include <../Plugins/Animation/MotionWarping/Source/MotionWarping/Public/MotionWarpingComponent.h>
#include <Animation/AnimMontage.h>



AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0, 0, 60));
	SpringArm->TargetArmLength = 150;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(SpringArm);

	ShootStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShootStartPoint"));
	ShootStartPoint->SetupAttachment(GetMesh());
	ShootStartPoint->SetRelativeLocation(FVector(-40, 20, 130));

	CenterArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterArrow"));
	CenterArrow->SetupAttachment(GetMesh());

	Left45DetectArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Left45DetectArrow"));
	Left45DetectArrow->SetRelativeLocation(FVector(0,-40,0));
	Left45DetectArrow->SetRelativeRotation(FRotator(0, -45, 0));
	Left45DetectArrow->SetupAttachment(GetCapsuleComponent());

	Right45DetectArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Right45DetectArrow"));
	Right45DetectArrow->SetRelativeLocation((FVector(0,40,0)));
	Right45DetectArrow->SetRelativeRotation(FRotator(0, 45, 0));
	Right45DetectArrow->SetupAttachment(GetCapsuleComponent());

	MotionWraping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWraping"));


	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);


	//Montage 
	ConstructorHelpers::FObjectFinder<UAnimMontage> TempMontage(TEXT("/Script/Engine.AnimMontage'/Game/4_SK/Animations/Sprint_To_Wall_Climb__1__Montage.Sprint_To_Wall_Climb__1__Montage'"));

	if (TempMontage.Succeeded())
	{
		ClimbMontageToPlay = TempMontage.Object;
	}
}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}

}

void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �޸��� <-> �ȱ� ��ȯ�� ������ 
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, Speed, 5*DeltaTime);

	TrackInteractable();
	if(isCovering) Covering();
	// ���� �� Trace Line Collision�� ������ش�.
	if(nowCovering) CoverMovement();
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);

	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);

	EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Triggered, this, &AMyPlayer::CoverCheck);

	EnhancedInputComponent->BindAction(VaultAction, ETriggerEvent::Triggered, this, &AMyPlayer::Vault);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMyPlayer::Run);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyPlayer::Run);
}

void AMyPlayer::Move(const FInputActionValue& value)
{
	FVector2D MovementVector = value.Get<FVector2D>();

	if (Controller != nullptr && !nowCovering)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	if (nowCovering)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		float deltaRotate = GetDeltaRotate();
		

		// W,S �Է�, ����� W,S �Է��� �ָ� �̷��.
		// ī�޶� ����� �¹���
		
		if (FMath::Abs(deltaRotate)<45)
		{
			if(canGoDetect)
			{
				AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
				UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);

			}
			else
			{
				if (MovementVector.Y < 0 && leftDetect)
				{
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
				else if (rightDetect && MovementVector.Y > 0)
				{
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
			}


			if (MovementVector.X != 0)
			{
				nowCovering = false;
			}
			
		}
		// ī�޶� ����� ������
		else if (FMath::Abs(deltaRotate) >135) {

			if (canGoDetect)
			{
				AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
				UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
			}
			else {

				if (MovementVector.Y > 0 && leftDetect)
				{
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
				else if (rightDetect && MovementVector.Y < 0)
				{
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
			}

			if (MovementVector.X != 0)
			{
				nowCovering = false;
			}
		}
		else 
		{
			if(deltaRotate>0) 
			{
				nowCovering = false;
				UE_LOG(LogTemp, Warning, TEXT("Cover End, %f"), deltaRotate);

			}
		}
	}
}

void AMyPlayer::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AMyPlayer::Vault(const FInputActionValue& value)
{
	// Motion Warp ����

	// LineTrace �߻�
	// �Է� ��
	FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector End = Start + GetCapsuleComponent()->GetForwardVector() * DistanceToObject;
	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	FHitResult HitResult2;
	
	bool LineHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		Start,
		End,
		TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true
	);
	
	if (LineHit)
	{
		// �����ϸ� Location�� Ȯ���ؾ��ϰ�, 
		// ��ü�� ���̸� Ȯ���ؾ� �Ѵ�. -> Vault ������ ����
		WallPos = HitResult.Location;
		FVector BoxOrigin;
		FVector BoxExtent;
		HitResult.GetActor()->GetActorBounds(false,BoxOrigin, BoxExtent);
		float SubHeight = BoxExtent.Z - HitResult.Location.Z;
		UE_LOG(LogTemp, Warning,TEXT("%f"), SubHeight);
		UE_LOG(LogTemp, Warning,TEXT("BOX %f"), BoxExtent.Z);
		UE_LOG(LogTemp, Warning,TEXT("HIT %f"), HitResult.Location.Z);
		if (SubHeight < VaultLimit)
		{// ������ Vault ���̶�� 
		// Vertical LineTrace�� ȣ���Ѵ�. 
			for(int i = 1; i<4; i++)
			{
				FVector VerticalStart = HitResult.Location + FVector(0, 0, VaultLimit) + GetActorForwardVector() * LineDelta * i;
				FVector VerticalEnd = VerticalStart - FVector(0, 0, 1000);
				bool VerticalHit = UKismetSystemLibrary::LineTraceSingle(
					this,
					VerticalStart,
					VerticalEnd,
					TraceTypeQuery3,
					false,
					ActorsToIgnore,
					EDrawDebugTrace::ForDuration,
					HitResult2,
					true
				);
				if(VerticalHit==false) 
				{
					LastPos = HitResult2.Location;
					canVault = true;
					canClimb = false;
					break;
				}
				else
				{
					if(i==1) StartPos = HitResult2.Location;
					if(i==2) MiddlePos = HitResult2.Location;
					if(i==3) {
						canVault = false;
						canClimb = true;
						LastPos = HitResult2.Location;
					}
				}
			}
		//Motion Warping(Mantling) �ִϸ��̼��� ȣ���Ѵ�. 
		
		/*GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		this->SetActorEnableCollision(false);
		MotionWraping->AddOrUpdateWarpTargetFromLocation(TEXT("HangingPos"), FVector(GetActorLocation().X, GetActorLocation().Y, StartPos.Z));

		PlayAnimMontage(ClimbMontageToPlay);
		
		this->SetActorEnableCollision(true);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);*/

		}
		else
		{
			canClimb = false;
		}
	}
	else
	{
		canClimb= false;
		canVault = false;
	}
}


void AMyPlayer::Run(const FInputActionValue& value)
{
	if(Speed > WalkSpeed) Speed = WalkSpeed;
	else Speed = RunSpeed;

}

float AMyPlayer::InteractStart_1Sec()
{
	FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
	GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, TEXT("InteractionStart"));
	GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, InteractionTimeString);

	if (InteractionTime > 1.f)
	{
		InteractionTime = 0.f;
		if (IsValid(CCTV))
		{
			CCTV->ActivateCCTV();
		}
		else
		{
			return InteractionTime;
		}
	}
	else
	{
		InteractionTime = InteractionTime + GetWorld()->GetDeltaSeconds();
	}

	return InteractionTime;
}

void AMyPlayer::InteractEnd_1Sec()
{
	InteractionTime = 0.f;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("InteractionEnd"));
}

void AMyPlayer::InteractionSinglePress()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Single Press"));
	if (IsValid(HackableActor))
	{
		HackableActor->Action_Interact();
	}
}

void AMyPlayer::TrackInteractable()
{
	FHitResult HitResult;
	FVector StartVector = PlayerCamera->GetComponentLocation();
	FVector EndVector = StartVector + PlayerCamera->GetForwardVector() * Distance;

	if (IsPossessing)
	{
		GetWorld()->LineTraceSingleByChannel(HitResult, StartVector, EndVector, ECollisionChannel::ECC_GameTraceChannel2);
		DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 0.001, 0, 4.f);

		if (IsValid(HitResult.GetActor()))
		{
			FString ObjName = HitResult.GetActor()->GetName();
			GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);

			CCTV = Cast<ACCTV>(HitResult.GetActor());
			HackableActor = Cast<AHackableActor>(HitResult.GetActor());
		}
	}
}

void AMyPlayer::CoverCheck(const FInputActionValue& value)
{	
	for (int i = 0; i < 3; i++)
	{
		bool RHit = ConverLineTrace(i*LineTraceDegree);
		// �浹ü�� �����Ǹ� LineTrace ����
		if (RHit) break;
		bool LHit = ConverLineTrace(-i*LineTraceDegree);
		if(LHit) break;
	}
}

void AMyPlayer::Shoot()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Fire"));
	
	FHitResult HitResult;
	FVector StartLocation = ShootStartPoint->GetComponentLocation();
	FVector EndLocation = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * ShootRange;

	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_EngineTraceChannel6);
	DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Black, false, 2);

	AddControllerYawInput(HorizontalRecoil);
	AddControllerPitchInput(VerticalRecoil);


	AAI_EnemyBase* Enemy = Cast<AAI_EnemyBase>(HitResult.GetActor());
	if(IsValid(Enemy))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Enemy Hit"));
		Enemy->SetAttack(this);
	}
}

void AMyPlayer::ZoomIn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Zoom"));
	isZooming = true;
	PlayerCamera->FieldOfView = FMath::Lerp<float>(90, 40, 0.9);


}

void AMyPlayer::ZoomOut()
{
	isZooming = false;
	PlayerCamera->FieldOfView = FMath::Lerp<float>(40, 90, 0.9);
}

void AMyPlayer::Covering()
{

	float temp = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
	//������ ��ġ�� �����ϸ� �����.
	if (DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() > 0.1)
	{
		// ������� ����ŭ �ٽ� �ʱ�ȭ
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
		AddMovementInput(HitActorOrigin - GetMesh()->GetComponentLocation());
		
	}
	else
	{
		isCovering = false;
		nowCovering = true;
		if (CoverObjectNormal.Length() != 0)
		{
			this->SetActorRotation(CoverObjectNormal.Rotation());
			//Rotation�� ���� �� Normal Vector�� Orthogonal Vector�� ������ش�. For Movement
			CoverObjectOrthogonal = FVector::CrossProduct(CoverObjectNormal, GetMesh()->GetUpVector());
		}
	}
	
}

bool AMyPlayer::ConverLineTrace(float degree)
{
	// Line Trace �Է� ��
	FVector LineTraceStart = PlayerCamera->GetComponentLocation();
	FRotator Rot (0.f, degree, 0.f);
	FVector RotVector = UKismetMathLibrary::Quat_RotateVector(Rot.Quaternion(), PlayerCamera->GetForwardVector()) * 500;
	FVector LineTraceEnd = LineTraceStart + RotVector;
	TArray<AActor*> ActorsToIgnore;

	// Line Trace ��°�
	FHitResult LineHitResult;

	// LineTrace 
	bool Hit = UKismetSystemLibrary::LineTraceSingle(
		this,
		LineTraceStart,
		LineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		LineHitResult,
		true
	);

	if(Hit) 
	{
		//���� Object �ĺ��� �Ǿ��ٸ� ���������� �Ҵ����ش�.
		CoverLineHitResult = LineHitResult;
		// Object�� Normal Vector�� �����´�. 
		CoverObjectNormal = LineHitResult.Normal;
		CoverObjectNormal.Normalize();

		// Object�� (��ġ,ũ��) ���
		LineHitResult.GetActor()->GetActorBounds(false, HitActorOrigin, HitActorExtent);
		HitActorOrigin = LineHitResult.Location;
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
		// ���� �����ϴ� ������ �õ��ϹǷ� true�� ������ ���ش�. 
		isCovering = true;
		return true;
	}
	else return false;
}

void AMyPlayer::CoverMovement()
{

	TArray<AActor*> ActorsToIgnore;

	// Box Trace �Է� ��
	FVector TraceStart = GetMesh()->GetComponentLocation() + GetMesh()->GetRightVector() * 30;
	FRotator TraceOrient = UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal);
	FVector HalfSize = FVector(8,72,32);

	// Box Trace ��� ��
	FHitResult HitResult;

	// BoxTrace
	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		TraceStart,
		TraceStart,
		HalfSize,
		TraceOrient,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	// Box �浹�� �߻����� ���
	canGoDetect = bHit? true : false;
	

	// Sphere Collision ���� 
	FVector SphereTraceStart = GetCapsuleComponent()->GetComponentLocation();
	FHitResult SphereHitResult;

	bool SHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		SphereTraceStart,
		SphereTraceStart,
		40.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		SphereHitResult,
		true
	);

	attached = SHit ? true : false;

	FVector SphereLeftTraceStart = Left45DetectArrow->GetComponentLocation();
	FHitResult SphereLeftHitResult;

	bool LSHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		SphereLeftTraceStart,
		SphereLeftTraceStart,
		70.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		SphereLeftHitResult,
		true
	);

	leftDetect = LSHit ? true : false;

	FVector SphereRightTraceStart = Right45DetectArrow->GetComponentLocation();
	FHitResult SphereRightHitResult;

	bool RSHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		SphereRightTraceStart,
		SphereRightTraceStart,
		70.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		SphereRightHitResult,
		true
	);

	rightDetect = RSHit ? true : false;
}

float AMyPlayer::GetDeltaRotate()
{
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CameraRotation, UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));

	return Delta.Yaw;
}
