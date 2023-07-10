#include "HackableActor_Bomb.h"
#include "AI_EnemyBase.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

AHackableActor_Bomb::AHackableActor_Bomb()
{
}

void AHackableActor_Bomb::Action_Interact()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("Explosion"));

	FVector ExplosionLocation = GetActorLocation();

	//UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::
}
