#include "MyPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include <Components/CapsuleComponent.h>
#include "Kismet/KismetSystemLibrary.h"
#include "Math/Vector.h"

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

	if (isCovering)
	// 만약 엄폐를 시도하는중이라면
	{
		// 벽을 뚫고 갈 수는 없기 때문에 적당한 위치에 도착하면 멈춘다.
		if (DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() < 1)
		{
			// 가까워진 값만큼 다시 초기화
			DistanceToCoverObject = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
			AddMovementInput(NewLocation - GetMesh()->GetComponentLocation());
			if (HitActorExtent.Z > 100)
			{
				UE_LOG(LogTemp, Warning, TEXT("Standing Motion"));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Crouching Motion"));
			}
		}

		isCovering = false;
	}
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

void AMyPlayer::Cover(const FInputActionValue& value)
{
	// Trace 입력 값
	FVector TraceStart = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * 400;
	FRotator TraceOrient = PlayerCamera->GetComponentRotation();
	FVector HalfSize = FVector(300, 70, 100);
	TArray<AActor*> ActorsToIgnore;

	//Trace 출력 값
	FHitResult HitResult;
	//FVector HitActorOrigin;
	//FVector HitActorExtent;

	//// 처음에는 collision과의 거리를 알 수 없으니 200으로 초기화해준다.
	//float DistanceToCoverObject = 200;

	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
		this,
		TraceStart,
		TraceStart,
		HalfSize,
		TraceOrient,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		HitResult,
		true
		);
	
	// 충돌이 발생했을 경우
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// collision의 spec(위치,크기) 출력
		HitActor->GetActorBounds(false, HitActorOrigin,HitActorExtent);
		// 벡터의 내분점을 활용하여 엄폐할 위치를 찾는다.
		// 50은 임의의 값이다.
		NewLocation = HitActorOrigin* ((DistanceToCoverObject - 50) / DistanceToCoverObject) + PlayerCamera->GetComponentLocation() * (50 / DistanceToCoverObject);
		// + 10은 Tick에서 필요한 판별식(현재위치-과거위치>1)을 위해 진행한다. -> 실험 후 없애도 됌.  
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
	
		// 이후 엄폐하는 행위를 시도하므로 true로 변경을 해준다. 
		isCovering = true;
	}
}
