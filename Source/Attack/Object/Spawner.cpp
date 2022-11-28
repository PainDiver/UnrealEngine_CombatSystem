// FFill out your copyright notice in the Description page of Project Settings.
#include "Spawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "../GlobalSystem/AttackGameInstance.h"


// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_spawningArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningArea"));
	//_spawningArea->SetBoundsScale(_boxSize);
}


// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	StartSpawningLoop(_interval,0.f);
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASpawner::StartSpawningLoop(float interval,float firstDelay)
{
	GetWorldTimerManager().SetTimer(_spawningHandle, FTimerDelegate::CreateLambda(
		[=]() 
		{
			SpawnInBox();
		}
	),interval, true, firstDelay);
}


void ASpawner::StopSpawningLoop()
{
	GetWorldTimerManager().ClearTimer(_spawningHandle);
}


void ASpawner::SpawnInBox()
{
	if (_bNumberLimit)
	{
		if (_spawnedActors.Num() == 0)
		{
			goto INITIAL_SPAWN;
		}

		for (auto actor : _spawnedActors)
		{
			if (actor.Value->IsHidden() && actor.Value->IsActorTickEnabled())
			{
				FVector spawningPlace = UKismetMathLibrary::RandomPointInBoundingBox(_spawningArea->GetComponentLocation(), _spawningArea->GetScaledBoxExtent());
				_spawnedActors.Remove(actor.Key, actor.Value);
				_spawnedActors.Add(actor.Key,GAMEINSTANCE->SpawnActorByOrigin(actor.Key, actor.Value->StaticClass(), spawningPlace));
			}
		}
	}
	else
	{
	INITIAL_SPAWN:
		for (int i = 0; i < _maxActors; i++)
		{
			for (auto actor : _actorTypes)
			{
				FVector spawningPlace = UKismetMathLibrary::RandomPointInBoundingBox(_spawningArea->GetComponentLocation(), _spawningArea->GetScaledBoxExtent());
				_spawnedActors.Add(actor.Key, GAMEINSTANCE->SpawnActorByOrigin(actor.Key, actor.Value, spawningPlace));
				
			}
		}
	}
}
