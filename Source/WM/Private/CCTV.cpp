#include "CCTV.h"
#include "MyPlayer.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

ACCTV::ACCTV()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CollisionArea = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionArea->SetupAttachment(Camera);

	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);
}

void ACCTV::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCTV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackInteractable();
}

void ACCTV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACCTV::ActivateCCTV()
{
	bIsUsing = true;
	CollisionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	MyPlayerController->SetViewTargetWithBlend(this, 1.f);

	HackingTransition->SetVisibility(true);

	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACCTV::PossessCCTV, 1.05f, false);
}

void ACCTV::PossessCCTV()
{
	GetWorld()->GetFirstPlayerController()->Possess(this);
	HackingTransition->SetVisibility(false);
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("Possessed CCTV"));
}

void ACCTV::TrackInteractable()
{
	if (bIsUsing)
	{
		FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
		GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Green, InteractionTimeString);

		FHitResult HitResult;
		FVector StartLocation = Camera->GetComponentLocation();
		FVector EndLocation = StartLocation + Camera->GetForwardVector() * Distance;

		GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel2);
		if (IsValid(HitResult.GetActor()))
		{
			FString ObjName;
			ObjName.Append("Current is ");
			ObjName.Append(HitResult.GetActor()->GetName());

			GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);

			TrackedOtherCCTV = Cast<ACCTV>(HitResult.GetActor());
		}
	}
}

void ACCTV::InteractStart_1Sec()
{
	FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
	GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, InteractionTimeString);

	if (InteractionTime > 1.f)
	{
		InteractionTime = 0.f;
		if (IsValid(TrackedOtherCCTV))
		{
			TrackedOtherCCTV->ActivateCCTV();
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("Move to Other Camera"));
			CollisionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			bIsUsing = false;
		}
	}
	else
	{
		InteractionTime = InteractionTime + GetWorld()->GetDeltaSeconds();
	}
}

void ACCTV::InteractEnd_1Sec()
{
	InteractionTime = 0.f;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Second Reset"));
}

void ACCTV::InteractionSinglePress()
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Single Press"));
}

void ACCTV::Back2Player(AMyPlayer* SinglePlayer, APlayerController* PlayerController)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Back2Player"));
	PlayerController->SetViewTargetWithBlend(SinglePlayer, 1.f);
	HackingTransition->SetVisibility(true);
	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, SinglePlayer]()->void
		{
			bIsUsing = false;
			CollisionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetWorld()->GetFirstPlayerController()->Possess(SinglePlayer);
			HackingTransition->SetVisibility(false);
			
		}), 1.05f, false);
}

