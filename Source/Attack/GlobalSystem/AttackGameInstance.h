// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../Object/ObjectManager.h"
#include "AttackGameInstance.generated.h"

/**
 * 
 */
class USoundManager;
class UParticleManager;
class UCameraShakeManager;
class UMeshManager;
class UAnimationManager;
class UMaterialManager;
class UTimeManager;
class UAIManager;
class UBehaviorTree;

UCLASS()
class ATTACK_API UAttackGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init();

	USoundManager*					GetSoundManager() { return _soundManager; }
	UParticleManager*				GetParticleManager() { return _particleManager; }
	UMeshManager*					GetMeshManager() { return _meshManager; }
	UCameraShakeManager*			GetCameraShakeManager() { return _cameraShakeManager; }
	UAnimationManager*				GetAnimationManager() { return _animManager; }
	UMaterialManager*				GetMaterialManager() { return _materialManager; }
	UObjectManager*					GetObjectManager() { return _objectManager; }
	UTimeManager*					GetTimeManager() { return _timeManager; }
	UAIManager*						GetAIManager() { return _aiManager; }


	void							PlaySoundAtLocation(FName name, FVector loc, float volume =1.f, float pitch = 1.f, float startTime = 0.f);

	void							PlaySoundCue2D(FName name, FVector loc, float volume=1.f, float pitch=1.f, float startTime=0.f);

	void							PlayParticleAtLocation(FName name, FVector loc, FVector scale = FVector::OneVector, FRotator rot = FRotator::ZeroRotator);

	UParticleSystemComponent*		PlayParticleAtMesh(FName name, USceneComponent* mesh, FName socket = NAME_None, FVector offset = FVector::ZeroVector, FVector scale = FVector::OneVector, FRotator offrot = FRotator::ZeroRotator);

	void							PlayCameraShake(FName name);

	USkeletalMesh*					GetMesh(FName name);

	UMaterial*						GetMaterial(FName name);

	UMaterialInstance*				GetMaterialInstance(FName name);

	UCurveFloat*					GetCurveFloat(FName name);

	UBehaviorTree*					GetBehaviorTree(FName name);

	AActor* SpawnActorByOrigin(FName name,TSubclassOf<AActor> type, const FVector& Loc = FVector::ZeroVector, const FRotator& Rot = FRotator::ZeroRotator, const FVector& Scale = FVector::OneVector)
	{
		return _objectManager->SpawnPoolActorByOrigin(name,type, Loc, Rot, Scale);
	}

	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, AActor>::Value, T*>::type SpawnPoolActor(const FVector& Loc = FVector::ZeroVector, const FRotator& Rot = FRotator::ZeroRotator)
	{
		return _objectManager->SpawnPoolActor<T>(Loc, Rot);
	}

	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, AActor>::Value, void>::type TakeActor(T* actor)
	{
		_objectManager->TakeActor<T>(actor);
	}

private:

	USoundManager*			_soundManager;
	UParticleManager*		_particleManager;
	UCameraShakeManager*	_cameraShakeManager;
	UMeshManager*			_meshManager;
	UAnimationManager*		_animManager;
	UMaterialManager*		_materialManager;
	UObjectManager*			_objectManager;
	UTimeManager*			_timeManager;
	UAIManager*				_aiManager;
};
