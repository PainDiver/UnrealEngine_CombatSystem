// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManager.generated.h"

/**
 * 
 */

UCLASS()
class ATTACK_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USoundManager();


	void PlaySoundAtLocation(FName name, FVector loc, float volume = 1.f, float pitch =1.f, float startTime = 0.f);

	void PlaySoundCue2D(FName name, FVector loc, float volume = 1.f, float pitch = 1.f, float startTime = 0.f);

	USoundBase* GetSoundCue(FName name) { return _sounds[name]; }


	TMap<FName, USoundBase*> _sounds;
};
