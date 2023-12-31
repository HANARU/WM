#include "CCTV.h"
#include "MyPlayer.h"
#include "WM_Instance.h"
#include "AI_EnemyBase.h"
#include "HackableActor.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include <UMG/Public/Components/WidgetComponent.h>

ACCTV::ACCTV()
{
	PrimaryActorTick.bCanEverTick = true;

	BasePosition = CreateDefaultSubobject<USceneComponent>(TEXT("Base"));
	RootComponent = BasePosition;

	CameraSupport = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraSupport"));
	CameraSupport->SetupAttachment(RootComponent);
	CameraSupport->SetRelativeRotation(FRotator(0, -90, 0));
	CameraSupport->bOwnerNoSee = true;

	CameraBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraBody"));
	CameraBody->SetupAttachment(CameraSupport);
	CameraBody->SetRelativeLocation(FVector(0.001, 18.438, 2.657));
	CameraBody->bOwnerNoSee = true;

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
	CollisionArea->SetSphereRadius(64);

	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);

	ConstructorHelpers::FObjectFinder<UStaticMesh> SupportMesh(TEXT("/Script/Engine.StaticMesh'/Game/ModernCity/Meshes/SM_MSecurityCamera_B_Support.SM_MSecurityCamera_B_Support'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Script/Engine.StaticMesh'/Game/ModernCity/Meshes/SM_MSecurityCamera_B.SM_MSecurityCamera_B'"));

	if (SupportMesh.Succeeded())
	{
		CameraSupport->SetStaticMesh(SupportMesh.Object);
	}
	if (BodyMesh.Succeeded())
	{
		CameraBody->SetStaticMesh(BodyMesh.Object);
	}

	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidget"));
	InteractableWidget->SetupAttachment(SpringArm);
	
}

void ACCTV::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCTV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackInteractable();

	/*FString BoolString;
	BoolString.Append("Tracking : ");
	BoolString.Append(UKismetStringLibrary::Conv_BoolToString(bIsTrackingAI));
	GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Green, BoolString);*/
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

	MyPlayerController->SetViewTargetWithBlend(this, 0.5f);

	HackingTransition->SetVisibility(true);

	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACCTV::PossessCCTV, 0.55f, false);
}

void ACCTV::PossessCCTV()
{
	GetWorld()->GetFirstPlayerController()->Possess(this);
	HackingTransition->SetVisibility(false);
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("Possessed CCTV"));
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
		
		HitActor = HitResult.GetActor();
		if (HitActor)
		{
			FString ObjName;
			ObjName.Append("Current is ");
			ObjName.Append(HitActor->GetName());

			if (HitActor->IsA(ACCTV::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);
				TrackedOtherCCTV = Cast<ACCTV>(HitActor);
				bIsTrackingCCTV = true;
			}
			else if (HitActor->IsA(AHackableActor::StaticClass()))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);
				HackableActor = Cast<AHackableActor>(HitActor);
				bIsTrackingObject = true;
			}
			else if (HitActor->IsA(AAI_EnemyBase::StaticClass()))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);
				TrackedEnemy = Cast<AAI_EnemyBase>(HitActor);
				bIsTrackingAI = true;
			}
		}
		else
		{
			HitActor = nullptr;
			bIsTrackingAI = false;
			bIsTrackingObject = false;
			bIsTrackingCCTV = false;
		}
	}
}

void ACCTV::InteractStart_1Sec()
{
	FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, InteractionTimeString);

	if (InteractionTime > 1.f)
	{
		InteractionTime = 0.f;
		if (IsValid(TrackedOtherCCTV))
		{
			TrackedOtherCCTV->ActivateCCTV();
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("Move to Other Camera"));
			CollisionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			bIsUsing = false;
		}
		else if (IsValid(HackableActor))
		{
			InteractionTime = 0.f;
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
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Second Reset"));
}

void ACCTV::InteractionSinglePress()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Single Press"));
	if (IsValid(HackableActor))
	{
		HackableActor->Action_Interact_Single();
	}
}

void ACCTV::Back2Player(AMyPlayer* SinglePlayer, APlayerController* PlayerController)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Back2Player"));
	bIsTrackingAI = false;
	bIsTrackingObject = false;
	bIsTrackingCCTV = false;
	PlayerController->SetViewTargetWithBlend(SinglePlayer, 0.5f);
	HackingTransition->SetVisibility(true);
	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, SinglePlayer]()->void
		{
			bIsUsing = false;
			CollisionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			GetWorld()->GetFirstPlayerController()->Possess(SinglePlayer);
			HackingTransition->SetVisibility(false);
			
		}), 0.55f, false);
}

