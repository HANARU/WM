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
		// ���� ������ �׺�޽� ��������
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
					FNavLocation nearnavloc;
					naviSystem->ProjectPointToNavigation(Loc, nearnavloc);
					if (FVector(nearnavloc.Location.X - Loc.X, nearnavloc.Location.Y - Loc.Y, 0).Size() > 100)
						continue;
					Loc = nearnavloc.Location;
					DrawDebugSphere(GetWorld(), Loc, 30.f, 0, FColor::Blue, false, 1);
					TArray<AActor*> ActorsToIgnore;
					ActorsToIgnore.Add(this);
					FColor color = FColor::Red;
					FHitResult hitinfo;
					bool bHit = UKismetSystemLibrary::SphereTraceSingle(
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
					);
					if (!bHit)
						continue;
					AActor* HitActor = hitinfo.GetActor();
					if ((HitActor->IsA(AAI_EnemyBase::StaticClass()))
						|| (HitActor->IsA(AMyPlayer::StaticClass()))
						|| ((player->GetActorLocation() - Loc).Size() <= 500.f))
						continue;
					FVector normalloc = hitinfo.Location + hitinfo.Normal;
					FCollisionQueryParams QueryParams;
					FVector dir = player->GetActorLocation() - normalloc;
					dir.Normalize();
					QueryParams.AddIgnoredActor(this);
					FHitResult hitsecinfo;
					bool bSecHit = GetWorld()->LineTraceSingleByChannel(
						hitsecinfo,
						normalloc,
						normalloc + dir * 100,
						ECC_GameTraceChannel12,
						QueryParams
					);
					if (hitsecinfo.GetActor() != player && bSecHit && !hitsecinfo.GetActor()->IsA(AAI_EnemyBase::StaticClass()))
					{
						FVector origin;
						FVector boxextent;
						HitActor->GetActorBounds(false, origin, boxextent);
						bool bIsWall = (boxextent.Z > 100);
						PRINT_LOG(TEXT("%f"), boxextent.Z);
						DrawDebugSphere(GetWorld(), normalloc, 30.f, 0, FColor::Black, false, 1);
						FHideLoc hideloc;
						hideloc.Loc = normalloc;
						hideloc.bIsWall = bIsWall;
						coverArray.Add(hideloc);
					}
				}
			}
		}
		repeatTimer = 1;
	}
}

