#include "HackableActor_CCTV.h"
#include "MyPlayer.h"
#include "WM_Instance.h"
#include "AI_EnemyBase.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include <UMG/Public/Components/WidgetComponent.h>

AHackableActor_CCTV::AHackableActor_CCTV()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsSinglePress = false;

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

	CollisionArea->SetupAttachment(Camera);
	CollisionArea->SetSphereRadius(120);

	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);

	ConstructorHelpers::FObjectFinder<UStaticMesh> SupportMesh(TEXT("/Script/Engine.StaticMesh'/Game/ModernCity/Meshes/SM_MSecurityCamera_B_Support.SM_MSecurityCamera_B_Support'"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> CameraMesh(TEXT("/Script/Engine.StaticMesh'/Game/ModernCity/Meshes/SM_MSecurityCamera_B.SM_MSecurityCamera_B'"));

	if (SupportMesh.Succeeded())
	{
		CameraSupport->SetStaticMesh(SupportMesh.Object);
	}
	if (CameraMesh.Succeeded())
	{
		CameraBody->SetStaticMesh(CameraMesh.Object);
	}

	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidget"));
	InteractableWidget->SetupAttachment(CameraBody);

	ConstructorHelpers::FObjectFinder<UUserWidget> TempUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/2_BP/BP_UI/UI_FocusedInteractableActor.UI_FocusedInteractableActor'"));

	if (TempUI.Succeeded())
	{
		FocusedInteractable = TempUI.Object;
	}
}

void AHackableActor_CCTV::BeginPlay()
{
	Super::BeginPlay();
	InteractableWidget->SetVisibility(false);
}

void AHackableActor_CCTV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackInteractable();
}

void AHackableActor_CCTV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AHackableActor_CCTV::ActivateCCTV()
{
	bIsUsing = true;
	CollisionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(this, 0);

	Player2CCTV = LoadObject<USoundWave>(nullptr, TEXT("/Script/Engine.SoundWave'/Game/5_Sound/SFX/SC_Player2CCTV.SC_Player2CCTV'"));
	UGameplayStatics::PlaySound2D(GetWorld(), Player2CCTV);

	MyPlayerController->SetViewTargetWithBlend(this, 1.f);

	HackingTransition->SetVisibility(true);

	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AHackableActor_CCTV::PossessCCTV, 1.05f, false);
}

void AHackableActor_CCTV::PossessCCTV()
{
	GetWorld()->GetFirstPlayerController()->Possess(this);
	HackingTransition->SetVisibility(false);
}

void AHackableActor_CCTV::TrackInteractable()
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
			if (HitActor->IsA(AHackableActor_CCTV::StaticClass()))
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, HitActor->GetName());
				TrackedOtherCCTV = Cast<AHackableActor_CCTV>(HitActor);
				OnInteractionCCTV(HitResult);
				TrackedOtherCCTV->CameraSupport->SetRenderCustomDepth(true);
				TrackedOtherCCTV->CameraBody->SetRenderCustomDepth(true);
				OtherCCTVUI = TrackedOtherCCTV->InteractableWidget;
				OtherCCTVUI->SetVisibility(true);
				bIsTrackingCCTV = true;
				return;
			}
			else if (HitActor->IsA(AHackableActor::StaticClass()) && HitActor != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, HitActor->GetName());
				HackableActor = Cast<AHackableActor>(HitActor);
				OnInteractionObject(HitResult);
				HackableActor->BodyMesh->SetRenderCustomDepth(true);
				bIsTrackingObject = true;
				return;
			}
			
			else if (HitActor->IsA(AAI_EnemyBase::StaticClass()))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);
				TrackedEnemy = Cast<AAI_EnemyBase>(HitActor);
				OnInteractionAI(HitResult);
				TrackedEnemy->GetMesh()->SetRenderCustomDepth(true);
				bIsTrackingAI = true;
				return;
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
	EndInteraction();
	if (OtherCCTVUI)	OtherCCTVUI->SetVisibility(false);
}

void AHackableActor_CCTV::InteractStart_1Sec()
{
	FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, InteractionTimeString);

	if (InteractionTime > 1.f)
	{
		InteractionTime = 0.f;
		if (IsValid(TrackedOtherCCTV) && bIsTrackingCCTV)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, TEXT("Move to Other Camera"));
			TrackedOtherCCTV->ActivateCCTV();
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

void AHackableActor_CCTV::InteractEnd_1Sec()
{
	InteractionTime = 0.f;
}

void AHackableActor_CCTV::InteractionSinglePress()
{
	if (IsValid(HackableActor) && TrackedOtherCCTV == nullptr)
	{
		HackableActor->Action_Interact_Single();
	}
	else if (IsValid(TrackedEnemy) && TrackedOtherCCTV == nullptr)
	{
		if (TrackedEnemy->AI_Num == TEXT("2"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Target Hacked"));
			OnTargetHacked();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Hacked"));
		}
	}
}

void AHackableActor_CCTV::Back2Player(AMyPlayer* SinglePlayer, APlayerController* PlayerController)
{
	bIsTrackingAI = false;
	bIsTrackingObject = false;
	bIsTrackingCCTV = false;

	OnDisableProcess();

	CCTV2Player = LoadObject<USoundWave>(nullptr, TEXT("/Script/Engine.SoundWave'/Game/5_Sound/SFX/SC_CCTV2Player.SC_CCTV2Player'"));
	UGameplayStatics::PlaySound2D(GetWorld(), CCTV2Player);

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
