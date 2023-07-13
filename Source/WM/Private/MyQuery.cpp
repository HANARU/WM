// Fill out your copyright notice in the Description page of Project Settings.


#include "MyQuery.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI_EnemyBase.h"
#include "../WM.h"

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

}

// Called every frame
void AMyQuery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	repeatTimer = FMath::Max(0, repeatTimer - DeltaTime);
	if (repeatTimer == 0)
	{
		TArray<AActor*> actors;
		AMyPlayer* player = Cast<AMyPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), AMyPlayer::StaticClass()));
		SetActorLocation(player->GetActorLocation());
		coverArray.Empty();
		if (player)
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					FVector Loc = GetActorLocation() + FVector(100 * (-width / 2 + i), 100 * (-height / 2 + j), 0);
					TArray<AActor*> ActorsToIgnore;
					ActorsToIgnore.Add(this);
					FColor color = FColor::Red;
					FHitResult hitinfo;
					bool bHit = UKismetSystemLibrary::SphereTraceSingle(
						GetWorld(),
						Loc,
						Loc,
						70,
						TraceTypeQuery11,
						false,
						ActorsToIgnore,
						EDrawDebugTrace::None,
						hitinfo,
						true
					);
					if (bHit)
					{
						//PRINT_LOG(TEXT("%f"), (player->GetActorLocation() - Loc).Size());
						//DrawDebugLine(GetWorld(), player->GetActorLocation(), Loc, FColor::Cyan, false, 1);
						AActor* HitActor = hitinfo.GetActor();
						if ((!HitActor->IsA(AAI_EnemyBase::StaticClass()))
							&& (!HitActor->IsA(AMyPlayer::StaticClass()))
							&& ((player->GetActorLocation() - Loc).Size() > 900.f))
						{
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
							//hitsecinfo.GetActor();
							//PRINT_LOG(hitsecinfo.GetActor()->GetName());
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
			}
		repeatTimer = 1;
	}
}

