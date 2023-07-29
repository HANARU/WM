// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuery.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI_EnemyBase.h"
#include "../WM.h"
#include "NavigationSystem.h"
// Sets default values
AMyQuery::AMyQuery()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyQuery::BeginPlay()
{
	Super::BeginPlay();
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		// 현재 월드의 네비메시 가져오기
		naviData = NavSystem->GetDefaultNavDataInstance(FNavigationSystem::ECreateIfEmpty::DontCreate);
		naviSystem = NavSystem;
	}
}

// Called every frame
void AMyQuery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	repeatTimer = FMath::Max(0, repeatTimer - DeltaTime);
	if (repeatTimer == 0)
	{
		AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
		SetActorLocation(player->GetActorLocation());
		coverArray.Empty();
		if (player && naviSystem)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					FVector Loc = GetActorLocation() + FVector(100 * (-width / 2 + i), 100 * (-height / 2 + j), 0);
					FHitResult groundinfo;
					FCollisionQueryParams groundParams;
					groundParams.AddIgnoredActor(this);
					bool bGroundHit = GetWorld()->LineTraceSingleByChannel(
						groundinfo,
						Loc + FVector(0, 0, 1000),
						Loc + FVector(0, 0, -1000),
						ECC_GameTraceChannel12,
						groundParams
					);
					if (bGroundHit)
					{
						Loc = groundinfo.Location;
					}
					FNavLocation nearnavloc;
					naviSystem->ProjectPointToNavigation(Loc, nearnavloc);
					if (FVector(nearnavloc.Location.X - Loc.X, nearnavloc.Location.Y - Loc.Y, 0).Size() > 100)
						continue;
					if ((player->GetActorLocation() - Loc).Size() <= 500.f)
						continue;
					Loc = nearnavloc.Location;
					/*TArray<AActor*> ActorsToIgnore;
					ActorsToIgnore.Add(this);
					FColor color = FColor::Red;
					FHitResult hitinfo;*/
					/*bool bHit = UKismetSystemLibrary::SphereTraceSingle(
						GetWorld(),
						Loc,
						Loc,
						100,
						TraceTypeQuery11,
						false,
						ActorsToIgnore,
						EDrawDebugTrace::None,
						hitinfo,
						true
					);*/
					/*if (!bHit)
						continue;*/
					//AActor* HitActor = hitinfo.GetActor();
					/*if ((HitActor->IsA(AAI_EnemyBase::StaticClass()))
						|| (HitActor->IsA(AMyPlayer::StaticClass())))
						continue;*/
					//DrawDebugSphere(GetWorld(), Loc, 30.f, 0, FColor::Blue, false, 1);
					FCollisionQueryParams QueryParams;
					FVector dir = player->GetActorLocation() - Loc;
					dir.Normalize();
					FVector xydir = dir;
					xydir.Z = 0;
					QueryParams.AddIgnoredActor(this);
					FHitResult LineHitResult;
					bool bSecHit = GetWorld()->LineTraceSingleByChannel(
						LineHitResult,
						Loc,
						Loc + xydir * 150,
						ECC_GameTraceChannel12,
						QueryParams
					);
					AActor* HitActor = LineHitResult.GetActor();
					if (HitActor != player && bSecHit && !HitActor->IsA(AAI_EnemyBase::StaticClass()))
					{
						FVector origin;
						FVector boxextent;
						HitActor->GetActorBounds(false, origin, boxextent);
						bool bIsWall = (boxextent.Z > 70);
						//PRINT_LOG(TEXT("%f"), boxextent.Z);
						FHideLoc hideloc;
						FHitResult hitoneresult;
						FVector rightVec = FVector::CrossProduct(LineHitResult.Normal, FVector::UpVector);
						bool bOneCheck = GetWorld()->LineTraceSingleByChannel(
							hitoneresult,
							Loc + rightVec * 100,
							Loc + xydir * 200 + rightVec * 100,
							ECC_GameTraceChannel12,
							QueryParams
						);
						FHitResult hittworesult;
						FVector leftVec = FVector::CrossProduct(LineHitResult.Normal, FVector::DownVector);
						bool bTwoCheck = GetWorld()->LineTraceSingleByChannel(
							hittworesult,
							Loc + leftVec * 100,
							Loc + xydir * 200 + leftVec * 100,
							ECC_GameTraceChannel12,
							QueryParams
						);
						if (bOneCheck)
						{
							//DrawDebugLine(GetWorld(), Loc, Loc + rightVec * 100, FColor::Blue, false, 1);
							hideloc.openType = 1;
						}
						if (bTwoCheck)
						{
							//DrawDebugLine(GetWorld(), Loc, Loc + leftVec * 100, FColor::Red, false, 1);
							if(hideloc.openType == 1)
								hideloc.openType = 3;
							else
								hideloc.openType = 2;
						}
						if (!(bOneCheck ^ bTwoCheck))
						{
							continue;
						}
							//DrawDebugSphere(GetWorld(), Loc, 50.f, 0, FColor::Green, false, 1);
						//DrawDebugSphere(GetWorld(), Loc, 60.f, 10, FColor::Green, false, 1);
						hideloc.Loc = Loc;
						hideloc.bIsWall = bIsWall;
						coverArray.Add(hideloc);
					}
				}
			}
		}
		repeatTimer = 1;
	}
}

