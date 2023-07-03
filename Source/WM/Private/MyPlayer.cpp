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
	// ���� ���� �õ��ϴ����̶��
	{
		float temp = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
		//����׿� ��������.
		//UE_LOG(LogTemp, Warning, TEXT("Distance : %f"), temp);
		//UE_LOG(LogTemp, Warning, TEXT("DistanceToCoverObject : %f"), DistanceToCoverObject);
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
		}

		
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
	// Trace �Է� ��
	FVector TraceStart = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * 400;
	FRotator TraceOrient = PlayerCamera->GetComponentRotation();
	FVector HalfSize = FVector(300, 70, 100);
	TArray<AActor*> ActorsToIgnore;

	//Trace ��� ��
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
		EDrawDebugTrace::Persistent,
		HitResult,
		true
		);
	
	// �浹�� �߻����� ���
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// collision�� spec(��ġ,ũ��) ���
		HitActor->GetActorBounds(false, HitActorOrigin,HitActorExtent);
		// ������ �������� Ȱ���Ͽ� ������ ��ġ�� ã�´�.
		// 50�� ������ ���̴�.
		NewLocation = HitActorOrigin * ((DistanceToCoverObject - 50) / DistanceToCoverObject) + PlayerCamera->GetComponentLocation() * (50 / DistanceToCoverObject);
		// + 10�� Tick���� �ʿ��� �Ǻ���(������ġ-������ġ>1)�� ���� �����Ѵ�. -> ���� �� ���ֵ� ��.  
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
		// ���� �����ϴ� ������ �õ��ϹǷ� true�� ������ ���ش�. 
		isCovering = true;
	}
}
