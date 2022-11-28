// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LegacyCameraShake.h"
#include "CameraShakeManager.generated.h"

/**
 * 
 */

UCLASS()
class ATTACK_API UAttackCameraShake : public ULegacyCameraShake
{
	GENERATED_BODY()

public:
	UAttackCameraShake();

};


UCLASS()
class ATTACK_API UCameraShakeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UCameraShakeManager();

	void PlayCameraShake(FName name);

	TSubclassOf<ULegacyCameraShake> GetCameraShake(FName name)
	{ 
		if(_cameraShakes.Contains(name))
			return _cameraShakes[name]; 
		return nullptr;
	}

private:

	TMap<FName, TSubclassOf<ULegacyCameraShake>> _cameraShakes;
};


