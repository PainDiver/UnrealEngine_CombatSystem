// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"


class UBoxComponent;

UCLASS()
class ATTACK_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void	BeginPlay() override;

public:	
	// Called every frame
	virtual void	Tick(float DeltaTime) override;

	void			StartSpawningLoop(float interval, float firstDelay);

	void			StopSpawningLoop();

	void			SpawnInBox();

private:

	FTimerHandle _spawningHandle;

	TMultiMap<FName, AActor*> _spawnedActors;

	float _boxSize = 1.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Actor Pool")
		bool _bNumberLimit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Actor Pool")
		float _interval = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Actor Pool")
		int32 _maxActors = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Actor Pool")
		TMap<FName, TSubclassOf<AActor>> _actorTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* _spawningArea;

};
