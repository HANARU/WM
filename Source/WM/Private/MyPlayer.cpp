#include "MyPlayer.h"
#include "Math/Vector.h"
#include "CCTV.h"
#include "AI_EnemyBase.h"
#include "HackableActor.h"
#include "HackableActor_CCTV.h"
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
#include <Kismet/GameplayStatics.h>
#include "AC_AI_Hp.h"
#include <UMG/Public/Blueprint/UserWidget.h>
#include <Components/PawnNoiseEmitterComponent.h>
#include <Particles/ParticleSystem.h>
#include <UMG/Public/Components/WidgetComponent.h>


AMyPlayer::AMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempObject(TEXT("/Script/Engine.SkeletalMesh'/Game/4_SK/Aiden/Aiden.Aiden'"));

	if (TempObject.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempObject.Object);
		GetMesh()->SetRelativeLocation(FVector(0,0,-90));
		GetMesh()->SetRelativeRotation(FRotator(0,0,-90));
		GetMesh()->SetRelativeScale3D(FVector(0.5));	
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0, 0, 60));
	SpringArm->TargetArmLength = 150;
	SpringArm->SocketOffset.Y = 55;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(SpringArm);

	ShootStartPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShootStartPoint"));
	ShootStartPoint->SetupAttachment(GetMesh());
	ShootStartPoint->SetRelativeLocation(FVector(-40, 20, 130));

	Pistol = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pistol"));
	Pistol->SetupAttachment(GetMesh(),TEXT("GunBody"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempPistol(TEXT("/Script/Engine.StaticMesh'/Game/3_SM/Pistol/Makarov/makarov.makarov'"));
	if (TempPistol.Succeeded())
	{
		Pistol->SetStaticMesh(TempPistol.Object);
		Pistol->SetRelativeLocation(FVector(-25.4,14,4.64));
		Pistol->SetRelativeRotation(FRotator(0,0.156,-94));
		Pistol->SetRelativeScale3D(FVector(0.3));
	}

	M416 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("M416"));
	M416->SetupAttachment(GetMesh(), TEXT("GunBody"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRifle(TEXT("/Script/Engine.StaticMesh'/Game/3_SM/M416/M4A1.M4A1'"));
	if (TempPistol.Succeeded())
	{
		M416->SetStaticMesh(TempRifle.Object);
		M416->SetRelativeLocation(FVector(-60, 5, -17));
		M416->SetRelativeRotation(FRotator(0.03, 1.53, 175));
		M416->SetRelativeScale3D(FVector(1.5));
	}

	CubeForAim = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeForAim"));
	CubeForAim->SetupAttachment(M416, TEXT("firePosition"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempCube(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (TempCube.Succeeded())
	{
		CubeForAim -> SetStaticMesh(TempCube.Object);
		CubeForAim -> SetRelativeLocation(FVector(0,0,2500));
		CubeForAim -> SetRelativeScale3D(FVector(0.1,0.1,50));
	}

	SphereForAim = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereForAim"));
	SphereForAim->SetupAttachment(CubeForAim);

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSphere(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (TempSphere.Succeeded())
	{
		SphereForAim->SetStaticMesh(TempSphere.Object);
		SphereForAim->SetRelativeLocation(FVector(0, 0, 50));
		SphereForAim->SetRelativeScale3D(FVector(3.0, 3.0, 0.03));
	}

	CubeForAimP = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeForAimP"));
	CubeForAimP->SetupAttachment(Pistol, TEXT("FirePosition"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempCubeP(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (TempCube.Succeeded())
	{
		CubeForAimP->SetStaticMesh(TempCubeP.Object);
		CubeForAimP->SetRelativeLocation(FVector(2500, 0, 0));
		CubeForAimP->SetRelativeScale3D(FVector(50, 0.1, 0.1));
	}

	SphereForAimP = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereForAimP"));
	SphereForAimP->SetupAttachment(CubeForAimP);

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempSphereP(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	if (TempSphereP.Succeeded())
	{
		SphereForAimP->SetStaticMesh(TempSphereP.Object);
		SphereForAimP->SetRelativeLocation(FVector(0, 0, 50));
		SphereForAimP->SetRelativeScale3D(FVector(3.0, 3.0, 0.03));
	}


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

	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
	PawnNoiseEmitter->bAutoActivate = true;

	HackingTransition = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	HackingTransition->SetVisibility(false);


	// Montage 
	/*ConstructorHelpers::FObjectFinder<UAnimMontage> TempMontage(TEXT("/Script/Engine.AnimMontage'/Game/4_SK/Animations/Sprint_To_Wall_Climb__1__Montage.Sprint_To_Wall_Climb__1__Montage'"));

	if (TempMontage.Succeeded())
	{
		ClimbMontageToPlay = TempMontage.Object;
	}*/

	// UMG
	ConstructorHelpers::FObjectFinder<UUserWidget> TempUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/2_BP/BP_UI/UI_FocusedInteractableActor.UI_FocusedInteractableActor'"));
	
	if (TempUI.Succeeded())
	{
		FocusedInteractable = TempUI.Object;
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> TempParticle(TEXT("/Script/Engine.ParticleSystem'/Game/6_MISC/FX/MuzzleFire/FX_Monsters/FX_Monster_Chicken/P_ChickenMaster_MuzzleFlash_01.P_ChickenMaster_MuzzleFlash_01'"));
	if (TempParticle.Succeeded())
	{
		FireEffect = TempParticle.Object;
	}

}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	//FocusedInteractable = CreateWidget<UUserWidget>(GetWorld(),)
	
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

	//<------------About Hacking--------------->//
	FillHackableCount(DeltaTime);
	TrackInteractable();

	// 달리기 <-> 걷기 전환시 디테일 
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, Speed, 5*DeltaTime);

	if(isCovering) Covering();
	// 엄폐 시 Trace Line Collision을 만들어준다.
	if(nowCovering) {
		CoverMovement();
		SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, 250, 5*DeltaTime);
	}
	else {
		SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, 150, 5*DeltaTime);
	}


}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPlayer::Move);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPlayer::MoveStop);

	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPlayer::Look);

	EnhancedInputComponent->BindAction(CoverAction, ETriggerEvent::Triggered, this, &AMyPlayer::CoverCheck);

	EnhancedInputComponent->BindAction(VaultAction, ETriggerEvent::Triggered, this, &AMyPlayer::Vault);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AMyPlayer::Run);

	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AMyPlayer::Stop);

	//EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Triggered, this, &AMyPlayer::ChangeGun);
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


	// 엄폐 Movement 
	if (nowCovering)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		float deltaRotate = GetDeltaRotate();
		

		// W,S 입력, 엄폐시 W,S 입력이 주를 이룬다.
		// 카메라 방향과 맞방향
		
		if (FMath::Abs(deltaRotate)<35)
		{
			if(canGoDetect)
			{
				if (MovementVector.Y > 0) {
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));
				}
				else if (MovementVector.Y < 0) {
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(-CoverObjectOrthogonal));
				}
				AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
			}
			else
			{
				if (MovementVector.Y < 0 && leftDetect)
				{
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(-CoverObjectOrthogonal));
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
				}
				else if (rightDetect && MovementVector.Y > 0)
				{
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
				}
				else if (attached && (canGoDetect ||(!leftDetect&&!rightDetect)))
				{
					AddMovementInput(CoverObjectOrthogonal, MovementVector.Y);
				}
			}


			if (MovementVector.X != 0)
			{
				nowCovering = false;
			}
			
		}
		// 카메라 방향과 역방향
		else if (FMath::Abs(deltaRotate) >145) {

			if (canGoDetect)
			{
				if (MovementVector.Y > 0) {
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(-CoverObjectOrthogonal));
				}
				else if(MovementVector.Y < 0) {
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));
				}
				AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
			}
			else {

				if (MovementVector.Y > 0 && leftDetect)
				{
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(-CoverObjectOrthogonal));
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
				}
				else if (rightDetect && MovementVector.Y < 0)
				{
					this->SetActorRotation(UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal));
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
				}
				else if (attached && (canGoDetect || (!leftDetect && !rightDetect)))
				{
					AddMovementInput(-CoverObjectOrthogonal, MovementVector.Y);
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
			}
		}
	}
}

void AMyPlayer::MoveStop(const FInputActionValue& value)
{
	PlayStopMontage();
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

	// LineTrace 발사
	// 입력 값
	FVector Start = GetCapsuleComponent()->GetComponentLocation() - FVector(0,0,90);
	StandUprototator = GetCapsuleComponent()->GetComponentRotation();
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
		// 성공하면 Location을 확인해야하고, 
		// 물체의 높이를 확인해야 한다. -> Vault 범위를 위해
		WallPos = HitResult.Location;
		FVector BoxOrigin;
		FVector BoxExtent;
		HitResult.GetActor()->GetActorBounds(false,BoxOrigin, BoxExtent);
		float SubHeight = BoxExtent.Z - HitResult.Location.Z;
		if (SubHeight < VaultLimit)
		{// 적당한 Vault 높이라면 
		// Vertical LineTrace를 호출한다. 
			DisableInput(GEngine->GetFirstLocalPlayerController(GetWorld()));	
			for(int i = 1; i<4; i++)
			{
				FVector VerticalStart = HitResult.Location + FVector(0, 0, VaultLimit) + GetActorForwardVector() * LineDelta * pow(i,1.6);
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
					canVault = true;
					canClimb = false;

					// 마지막에 찍힌 Temp Location에서 Z축으로 LineTrace를 쏜다.
					// LineTrace와 바닥에 맞은 지점의 값을 LastPos 값에 넣어준다.
					if (i != 1)
					{
						FVector LastVerticalStart = TempPos + GetCapsuleComponent()->GetForwardVector() * 80;
						FVector LastVerticalEnd = LastVerticalStart - FVector(0, 0, 10000);
						bool LastVerticalHit = UKismetSystemLibrary::LineTraceSingle(
							this,
							LastVerticalStart,
							LastVerticalEnd,
							TraceTypeQuery1,
							false,
							ActorsToIgnore,
							EDrawDebugTrace::ForDuration,
							HitResult2,
							true
						);

						if(LastVerticalHit) LastPos = HitResult2.Location;
					}
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
					TempPos = HitResult2.Location;
				}
			}
			if(canClimb) ClimbMotionWarp();
			else if(canVault) VaultMotionWarp();
			else EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		//Motion Warping(Mantling) 애니메이션을 호출한다. 
		
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
	if(nowCovering) Speed = 350.f;
	else 
	{
		Speed = RunSpeed;
		
	}
}

void AMyPlayer::Stop(const FInputActionValue& value)
{
	if(nowCovering) Speed = 250.f;
	else 
	{
		Speed = WalkSpeed;
	}
}

void AMyPlayer::ChangeGun()
{
	if (isArmed)
	{
		if (isRifle)
		{
			isRifle = false;
			M416->SetVisibility(false);
			isPistol = true;
			Pistol->SetVisibility(true);
		}
		else if (isPistol)
		{
			isPistol = false;
			Pistol->SetVisibility(false);
			isArmed = false;
		}
	}
	else {
		isArmed = true;
		isRifle = true;
		M416->SetVisibility(true);
	}
}

void AMyPlayer::FillHackableCount(float DeltaTime)
{
	CurrentTime += DeltaTime;

	if (CurrentTime > FillHackableCountTime && HackableCount < HackableMaxCount)
	{
		HackableCount++;
		CurrentTime = 0;
	}
}

float AMyPlayer::InteractStart_1Sec()
{
	FString InteractionTimeString = FString::SanitizeFloat(InteractionTime);
	//GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, TEXT("InteractionStart"));
	GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Blue, InteractionTimeString);

	if (InteractionTime > 1.f)
	{
		InteractionTime = 0.f;
		if (IsValid(CCTV))
		{
			CCTV->ActivateCCTV();
		}
		else if (IsValid(HackedCCTV))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Possess"));
			HackedCCTV->ActivateCCTV();
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
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("InteractionEnd"));
}

void AMyPlayer::InteractionSinglePress()
{
	//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Single Press"));
	if (IsValid(HackableActor))
	{
		HackableActor->Action_Interact_Single();
		if (HackableActor->bNeed2Subtract)			// 폭발물, 프로텍터 사용 시
		{
			HackableCount--;
		}
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
		DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Red, false, 0.001, 0, 10.f);

		if (IsValid(HitResult.GetActor()))
		{
			FString ObjName;
			ObjName.Append("Current is ");
			ObjName.Append(HitResult.GetActor()->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 0.001, FColor::Red, ObjName);

			if (HitResult.GetActor()->IsA(ACCTV::StaticClass()))
			{
				CCTV = Cast<ACCTV>(HitResult.GetActor());
			}

			else if (HitResult.GetActor()->IsA(AHackableActor_CCTV::StaticClass()))
			{
				HackedCCTV = Cast<AHackableActor_CCTV>(HitResult.GetActor());
				OnInteractionCCTV(HitResult);
				CCTVUi = HackedCCTV->InteractableWidget;
				CCTVUi->SetVisibility(true);
				return;
			}
			else if(HitResult.GetActor()->IsA(AHackableActor::StaticClass()))
			{
				HackableActor = Cast<AHackableActor>(HitResult.GetActor());
				OnInteractionObject(HitResult);
				return;
			}
		}

		// 아무것도 맞지 않았다면
		EndInteraction();
		if (CCTVUi)	CCTVUi->SetVisibility(false);
	}
}

FVector AMyPlayer::CameraLineTrace()
{
	FVector CameraLineTraceStart = PlayerCamera->GetComponentLocation();
	FVector CameraLineTraceEnd = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * ShootRange;
	TArray<AActor*> ActorsToIgnore;

	// Line Trace 출력값
	FHitResult LineHitResult;
	FVector EndLocation = PlayerCamera->GetComponentLocation() + PlayerCamera->GetForwardVector() * ShootRange;

	// LineTrace 
	bool Hit = UKismetSystemLibrary::LineTraceSingle(
		this,
		CameraLineTraceStart,
		CameraLineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		LineHitResult,
		true
	);

	if (Hit)
	{// 적이 맞으면 적의 해당 위치가 EndLocation이된다.
		EndLocation = LineHitResult.Location;
	}

	return EndLocation;

}

void AMyPlayer::Shoot()
{
	//LineTrace 조정
	// Line Trace 입력 값	
	FHitResult HitResult;
	FVector StartLocation = isRifle ? M416->GetSocketLocation(FName("firePosition")) : Pistol->GetSocketLocation(FName("FirePosition"));
	FVector EndLocation = CameraLineTrace();


	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel2);
	//DrawDebugLine(GetWorld(), HitResult.TraceStart, HitResult.TraceEnd, FColor::Black, false, 2);


	// 반동
	/*AddControllerYawInput(HorizontalRecoil);
	AddControllerPitchInput(VerticalRecoil);*/

	FVector firePosition = isRifle ? M416->GetSocketLocation(FName("firePosition")) : Pistol->GetSocketLocation(FName("FirePosition"));
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireEffect, firePosition, (EndLocation - StartLocation).Rotation(), FVector(0.05));

	// 효과
	//PlayShootMontage();


	AAI_EnemyBase* Enemy = Cast<AAI_EnemyBase>(HitResult.GetActor());
	if(IsValid(Enemy))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, HitResult.GetComponent()->GetName());
		if (HitResult.GetComponent()->GetName() == "cap_head")
		{
			Enemy->SetDie();
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Enemy Hit"));

			Enemy->SetAttack(this);
			if (Enemy->hpComp)
			{
				Enemy->hpComp->OnHit(4);

			}
		}
	}
	
	

}

void AMyPlayer::ZoomIn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Zoom"));
	SpringArm->bEnableCameraLag = false;
	isZooming = true;
	PlayerCamera->FieldOfView = FMath::Lerp<float>(90, 40, 0.9);
	//SpringArm->SocketOffset.Y = -55;
	SpringArm->SetRelativeLocation(FVector(0,0,70));
	bUseControllerRotationYaw = true;
	if (nowCovering) SpringArm->TargetArmLength = 150;
	this->SetActorRotation(FRotator(0, (PlayerCamera->GetForwardVector()*100 - GetMesh()->GetForwardVector()).Rotation().Yaw ,0));

	// LineTrace 발사 for 조준선
	// Line Trace 입력 값	
	// 엄폐 상태가 아닐 때만 -> 가까이 있는 위치 조정한다. 
	if(!nowCovering)
	{
		FHitResult HitResult;
		FVector LineTraceStart = isRifle ? M416->GetSocketLocation(FName("firePosition")) : Pistol->GetSocketLocation(FName("FirePosition"));
		FVector LineTraceEnd = isRifle ?  SphereForAim->GetComponentLocation() : SphereForAimP->GetComponentLocation();
		TArray<AActor*> ActorsToIgnore;

		bool Hit = UKismetSystemLibrary::LineTraceSingle(
			this,
			LineTraceStart,
			LineTraceEnd,
			ETraceTypeQuery::TraceTypeQuery5,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		if (Hit)
		{
			SphereForAim->SetWorldLocation(HitResult.Location);
		}
		else {
			SphereForAim->SetRelativeLocation(FVector(0, 0, 50));
		}
	}

}

void AMyPlayer::ZoomOut()
{
	SpringArm->bEnableCameraLag = true;
	isZooming = false;
	PlayerCamera->FieldOfView = FMath::Lerp<float>(40, 90, 0.9);
	SpringArm->SetRelativeLocation(FVector(0,0,60));
	//SpringArm->SocketOffset.Y = 55;
	float deltaRotate = GetDeltaRotate();
	if(nowCovering)
	{// 플레이어가 바라본 상태로 다시 엄폐를 수행한다.
		if(FMath::Abs(deltaRotate)<90) this->SetActorRotation(FRotator(0, CoverObjectOrthogonal.Rotation().Yaw, 0));
		else this->SetActorRotation(FRotator(0, CoverObjectOrthogonal.Rotation().Yaw +180, 0));
	}
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
}

void AMyPlayer::CoverCheck(const FInputActionValue& value)
{
	for (int i = 0; i < 3; i++)
	{
		bool RHit = ConverLineTrace(i * LineTraceDegree);
		// 충돌체가 감지되면 LineTrace 중지
		if (RHit) break;
		bool LHit = ConverLineTrace(-i * LineTraceDegree);
		if (LHit) break;
	}
}

void AMyPlayer::Covering()
{

	float temp = DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
	//적당한 위치에 도착하면 멈춘다.
	if (DistanceToCoverObject - (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() > 0.1)
	{
		// 가까워진 값만큼 다시 초기화
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size();
		GetCharacterMovement()->MaxWalkSpeed = 600;
		AddMovementInput(HitActorOrigin - GetMesh()->GetComponentLocation());
		isCovering = true;
	}
	else
	{
		isCovering = false;
		nowCovering = true;
		if (CoverObjectNormal.Length() != 0)
		{
			CoverObjectOrthogonal = FVector::CrossProduct(CoverObjectNormal, GetMesh()->GetUpVector());
			this->SetActorRotation(FRotator(0, CoverObjectOrthogonal.Rotation().Yaw,0));
			//Rotation이 끝난 후 Normal Vector의 Orthogonal Vector을 만들어준다. For Movement
		}
		GetCharacterMovement()->MaxWalkSpeed = 300;
	}
	
}

bool AMyPlayer::ConverLineTrace(float degree)
{
	// Line Trace 입력 값
	FVector LineTraceStart = PlayerCamera->GetComponentLocation() - FVector(0,0,50);
	FRotator Rot (0.f, degree, 0.f);
	FVector RotVector = UKismetMathLibrary::Quat_RotateVector(Rot.Quaternion(), PlayerCamera->GetForwardVector()) * 800;
	FVector LineTraceEnd = LineTraceStart + RotVector;
	TArray<AActor*> ActorsToIgnore;

	// Line Trace 출력값
	FHitResult LineHitResult;

	// LineTrace 
	bool Hit = UKismetSystemLibrary::LineTraceSingle(
		this,
		LineTraceStart,
		LineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		LineHitResult,
		true
	);

	if(Hit) 
	{
		//만약 Object 식별이 되었다면 전역변수에 할당해준다.
		CoverLineHitResult = LineHitResult;
		// Object의 Normal Vector를 가져온다. 
		CoverObjectNormal = LineHitResult.Normal;
		CoverObjectNormal.Normalize();

		// Object의 (위치,크기) 출력
		LineHitResult.GetActor()->GetActorBounds(false, HitActorOrigin, HitActorExtent);
		HitActorOrigin = LineHitResult.Location;
		DistanceToCoverObject = (HitActorOrigin - GetMesh()->GetComponentLocation()).Size() + 10;
		// 이후 엄폐하는 행위를 시도하므로 true로 변경을 해준다. 
		isCovering = true;
		return true;
	}
	else return false;
}

void AMyPlayer::CoverMovement()
{

	TArray<AActor*> ActorsToIgnore;

	// Box Trace 입력 값
	FVector TraceStart = GetMesh()->GetComponentLocation() + GetMesh()->GetRightVector() * 70;
	FRotator TraceOrient = UKismetMathLibrary::MakeRotFromX(CoverObjectOrthogonal);
	FVector HalfSize = FVector(8,72,32);

	// Box Trace 출력 값
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

	// Box 충돌이 발생했을 경우
	canGoDetect = bHit? true : false;
	

	// Sphere Collision 관련 
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
		25.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
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
		25.f,
		ETraceTypeQuery::TraceTypeQuery3,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
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
