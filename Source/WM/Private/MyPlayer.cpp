#include "MyPlayer.h"
#include "Math/Vector.h"
#include "CCTV.h"
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

	CenterArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("CenterArrow"));
	CenterArrow->SetupAttachment(GetMesh());

	Left45DetectArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Left45DetectArrow"));
	Left45DetectArrow->SetRelativeRotation(FRotator(0, -45, 0));
	Left45DetectArrow->SetupAttachment(GetCapsuleComponent());

	Right45DetectArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Right45DetectArrow"));
	Right45DetectArrow->SetRelativeRotation(FRotator(0, 45, 0));
	Right45DetectArrow->SetupAttachment(GetCapsuleComponent());

	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);
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

	TrackInteractable();
	if(isCovering) CoverCheck();
	// ���� �� Trace Line Collision�� ������ش�.
	if(nowCovering) CoverMovement();
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);

	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);

	EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Triggered, this, &AMyPlayer::Cover);
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
				if (MovementVector.Y < 0 && rightDetect)
				{
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
				else if (leftDetect && MovementVector.Y > 0)
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

				if (MovementVector.Y > 0 && rightDetect)
				{
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
					UE_LOG(LogTemp, Warning, TEXT("%f"), deltaRotate);
				}
				else if (leftDetect && MovementVector.Y < 0)
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


void AMyPlayer::InteractStart_1Sec()
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
			return;
		}
	}
	else
	{
		InteractionTime = InteractionTime + GetWorld()->GetDeltaSeconds();
	}
}

void AMyPlayer::InteractEnd_1Sec()
{
	InteractionTime = 0.f;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("InteractionEnd"));
}

void AMyPlayer::InteractionSinglePress()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Single Press"));
	if (IsValid(InteractObject))
	{
		InteractObject->InteractCheck();
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
			InteractObject = Cast<AInteractableObject>(HitResult.GetActor());
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, TEXT("None"));
		}
	}
}

void AMyPlayer::Cover(const FInputActionValue& value)
{
	// Box Trace �Է� ��
	FVector TraceStart = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * 400;
	FRotator TraceOrient = PlayerCamera->GetComponentRotation();
	FVector HalfSize = FVector(300, 70, 100);
	TArray<AActor*> ActorsToIgnore;
	
	// Line Trace �Է� ��
	FVector LineTraceStart = GetArrowComponent()->GetComponentLocation();
	FVector LineTraceEnd = LineTraceStart + GetArrowComponent()->GetForwardVector() * 500;

	// Box Trace ��� ��
	FHitResult HitResult;
	
	// Line Trace ��°�
	FHitResult LineHitResult;

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
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
	);

	// Box �浹�� �߻����� ���
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// collision�� spec(��ġ,ũ��) ���
		HitActor->GetActorBounds(false, HitActorOrigin, HitActorExtent);
		// ������ �������� Ȱ���Ͽ� ������ ��ġ�� ã�´�.
		// 50�� ������ ���̴�.
		NewLocation = HitActorOrigin * ((DistanceToCoverObject - 50) / DistanceToCoverObject) + PlayerCamera->GetComponentLocation() * (50 / DistanceToCoverObject);
		// + 10�� Tick���� �ʿ��� �Ǻ���(������ġ-������ġ>1)�� ���� �����Ѵ�. -> ���� �� ���ֵ� ��.  
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
		// ���� �����ϴ� ������ �õ��ϹǷ� true�� ������ ���ش�. 
		isCovering = true;
	}

	// LineTrace 
	bool LHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		LineTraceStart,
		LineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		LineHitResult,
		true
	);

	// Line Hit �߻����� ���
	if (LHit)
	{
		CoverObjectNormal = LineHitResult.Normal;
		CoverObjectNormal.Normalize();
	}
}

void AMyPlayer::CoverCheck()
{

	float temp = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
	// ���� �հ� �� ���� ���� ������ ������ ��ġ�� �����ϸ� �����.
	if (DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() > 0.1)
	{
		// ������� ����ŭ �ٽ� �ʱ�ȭ
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
		AddMovementInput(NewLocation - GetMesh()->GetComponentLocation());
		if (HitActorExtent.Z > 100)
		{
			UE_LOG(LogTemp, Warning, TEXT("Standing Motion"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Crouching Motion"));
		}
	}
	else
	{
		isCovering = false;
		nowCovering = true;
		if (CoverObjectNormal.Length() != 0)
		{
			this->SetActorRotation(CoverObjectNormal.Rotation());
			//Rotation�� ���� �� Normal Vector�� Orthogonal Vector�� ������ش�.
			CoverObjectOrthogonal = FVector::CrossProduct(CoverObjectNormal, GetMesh()->GetUpVector());
		}
	}
	
}

void AMyPlayer::CoverMovement()
{

	// LeftBehind Line Trace �Է� ��
	FVector LeftLineTraceStart = Left45DetectArrow->GetComponentLocation() + Left45DetectArrow->GetForwardVector() * -75;
	FVector LeftLineTraceEnd = LeftLineTraceStart + Left45DetectArrow->GetForwardVector() * 150;
	TArray<AActor*> ActorsToIgnore;

	// LeftBehind Line Trace ��°�
	FHitResult LeftLineHitResult;

	// LeftBehind LineTrace 
	bool LHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		LeftLineTraceStart,
		LeftLineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		LeftLineHitResult,
		true
	);

	// Line Hit �߻����� ���
	if (LHit)
	{
		leftDetect = true;
	}
	else {
		leftDetect = false;
	}

	// RightBehind Line Trace �Է� ��
	FVector RightLineTraceStart = Right45DetectArrow->GetComponentLocation() + Right45DetectArrow->GetForwardVector() * -75;
	FVector RightLineTraceEnd = RightLineTraceStart + Right45DetectArrow->GetForwardVector() * 150;

	// RightBehind Line Trace ��°�
	FHitResult RightLineHitResult;

	// RightBehind LineTrace 
	bool RHit = UKismetSystemLibrary::LineTraceSingle(
		this,
		RightLineTraceStart,
		RightLineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		RightLineHitResult,
		true
	);

	// Line Hit �߻����� ���
	if (RHit)
	{
		rightDetect = true;
	}
	else {
		rightDetect = false;
	}

	UE_LOG(LogTemp,Warning,TEXT("%d, %d"), leftDetect, rightDetect);


	// Box Trace �Է� ��
	FVector TraceStart = GetMesh()->GetComponentLocation() + GetMesh()->GetRightVector() * 30;
	FRotator TraceOrient = UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal);
	FVector HalfSize = FVector(8,72,32);
	//TArray<AActor*> ActorsToIgnore;

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
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true
	);

	// Box �浹�� �߻����� ���
	if (bHit)
	{
		canGoDetect = true;
	}
	else
	{
		canGoDetect =false;
	}
}

float AMyPlayer::GetDeltaRotate()
{
	FRotator CameraRotation = PlayerCamera->GetComponentRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CameraRotation, UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));

	return Delta.Yaw;
}
