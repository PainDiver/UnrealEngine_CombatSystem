// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackGameInstance.h"
#include "../Sound/SoundManager.h"
#include "../FX/ParticleManager.h"
#include "../Camera/CameraShakeManager.h"
#include "../Mesh/MeshManager.h"
#include "../Animation/AnimationManager.h"
#include "../Material/MaterialManager.h"
#include "../Time/TimeManager.h"
#include "../AI/AIManager.h"

void UAttackGameInstance::Init()
{
	Super::Init();

	_soundManager = GetSubsystem<USoundManager>();
	_particleManager = GetSubsystem<UParticleManager>();
	_cameraShakeManager = GetSubsystem<UCameraShakeManager>();
	_meshManager = GetSubsystem<UMeshManager>();
	_animManager = GetSubsystem<UAnimationManager>();
	_materialManager = GetSubsystem<UMaterialManager>();
	_objectManager = GetSubsystem<UObjectManager>();
	_timeManager = GetSubsystem<UTimeManager>();
	_aiManager = GetSubsystem<UAIManager>();
}

void UAttackGameInstance::PlaySoundAtLocation(FName name, FVector loc, float volume, float pitch, float startTime)
{
	if (_soundManager)
	{
		_soundManager->PlaySoundAtLocation(name, loc, volume, pitch, startTime);
	}
}

void UAttackGameInstance::PlaySoundCue2D(FName name, FVector loc, float volume, float pitch, float startTime)
{
	if (_soundManager)
	{
		_soundManager->PlaySoundCue2D(name, loc, volume, pitch, startTime);
	}
}

void UAttackGameInstance::PlayParticleAtLocation(FName name, FVector loc, FVector scale, FRotator rot)
{
	if (_particleManager)
	{
		_particleManager->PlayParticleAtLocation(name, loc, scale, rot);
	}
}

UParticleSystemComponent* UAttackGameInstance::PlayParticleAtMesh(FName name, USceneComponent* mesh, FName socket, FVector offset, FVector scale, FRotator offrot)
{
	if (_particleManager)
	{
		return _particleManager->PlayParticleAtMesh(name, mesh, socket, offset, scale, offrot);
	}
	else
		return nullptr;
}

void UAttackGameInstance::PlayCameraShake(FName name)
{
	if (_cameraShakeManager)
	{
		_cameraShakeManager->PlayCameraShake(name);
	}
}

USkeletalMesh* UAttackGameInstance::GetMesh(FName name)
{
	return _meshManager->GetMesh(name);
}

UMaterial* UAttackGameInstance::GetMaterial(FName name)
{
	return _materialManager->GetMaterial(name);
}

UMaterialInstance* UAttackGameInstance::GetMaterialInstance(FName name)
{
	return _materialManager->GetMaterialInstance(name);
}

UCurveFloat* UAttackGameInstance::GetCurveFloat(FName name)
{
	return _timeManager->GetCurve(name);
}

UBehaviorTree* UAttackGameInstance::GetBehaviorTree(FName name)
{
	return _aiManager->GetBehaviorTree(name);
}
