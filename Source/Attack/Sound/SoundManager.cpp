// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


namespace RESOURCE
{
	const TMap<FName, const TCHAR*> SOUND_RESOURCE_PATHS = {
		TPair<FName,const TCHAR*>("AttackSound",TEXT("/Game/MyContent/Sound/AttackSound")),
	};
}

USoundManager::USoundManager()
{
	for (auto& namePath : RESOURCE::SOUND_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<USoundBase> sound(namePath.Value);
		if (sound.Succeeded())
		{
			_sounds.Add(namePath.Key, sound.Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Resource Not Found"), *namePath.Key.ToString());
		}
	}
}

void USoundManager::PlaySoundAtLocation(FName name, FVector loc,float volume,float pitch, float startTime)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), _sounds[name], loc, volume, pitch, startTime);
}

void USoundManager::PlaySoundCue2D(FName name, FVector loc, float volume, float pitch, float startTime)
{
	UGameplayStatics::PlaySound2D(GetWorld(), _sounds[name], volume, pitch, startTime);
}
