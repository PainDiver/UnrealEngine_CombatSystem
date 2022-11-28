// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraShakeManager.h"

UAttackCameraShake::UAttackCameraShake()
{
	OscillationDuration = 0.3f;
	RotOscillation.Pitch.Amplitude = 3.0f;
	RotOscillation.Pitch.Frequency = 15.f;

	RotOscillation.Yaw.Amplitude = 3.f;
	RotOscillation.Yaw.Frequency = 10.f;
}



UCameraShakeManager::UCameraShakeManager()
{
	_cameraShakes.Add("AttackCameraShake", UAttackCameraShake::StaticClass());
}

void UCameraShakeManager::PlayCameraShake(FName name)
{
	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(_cameraShakes[name]);
}
