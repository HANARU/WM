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

	CoverCheck();
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

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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
	// Trace 입력 값
	FVector TraceStart = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * 400;
	FRotator TraceOrient = PlayerCamera->GetComponentRotation();
	FVector HalfSize = FVector(300, 70, 100);
	TArray<AActor*> ActorsToIgnore;

	//Trace 출력 값
	FHitResult HitResult;


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

	// 충돌이 발생했을 경우
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// collision의 spec(위치,크기) 출력
		HitActor->GetActorBounds(false, HitActorOrigin, HitActorExtent);
		// 벡터의 내분점을 활용하여 엄폐할 위치를 찾는다.
		// 50은 임의의 값이다.
		NewLocation = HitActorOrigin * ((DistanceToCoverObject - 50) / DistanceToCoverObject) + PlayerCamera->GetComponentLocation() * (50 / DistanceToCoverObject);
		// + 10은 Tick에서 필요한 판별식(현재위치-과거위치>1)을 위해 진행한다. -> 실험 후 없애도 됌.  
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
		// 이후 엄폐하는 행위를 시도하므로 true로 변경을 해준다. 
		isCovering = true;
	}
}

void AMyPlayer::CoverCheck()
{
	if (isCovering)
		// 만약 엄폐를 시도하는중이라면
	{
		float temp = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
		//디버그용 지워도됌.
		//UE_LOG(LogTemp, Warning, TEXT("Distance : %f"), temp);
		//UE_LOG(LogTemp, Warning, TEXT("DistanceToCoverObject : %f"), DistanceToCoverObject);
		// 벽을 뚫고 갈 수는 없기 때문에 적당한 위치에 도착하면 멈춘다.
		if (DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() > 0.1)
		{
			// 가까워진 값만큼 다시 초기화
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
		}
	}
}