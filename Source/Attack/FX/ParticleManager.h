// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ParticleManager.generated.h"

/**
 * 
 */


UCLASS()
class ATTACK_API UParticleManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UParticleManager();

	void								PlayParticleAtLocation(FName name,FVector loc, FVector scale = FVector::OneVector, FRotator rot = FRotator::ZeroRotator);

	UParticleSystemComponent*			PlayParticleAtMesh(FName name,USceneComponent* mesh,FName socket,FVector offset = FVector::ZeroVector, FVector scale = FVector::OneVector,FRotator offrot = FRotator::ZeroRotator);

	UParticleSystem*					GetParticle(FName name) { return _particles[name]; }

private:
	TMap<FName, UParticleSystem*>		_particles;
};
