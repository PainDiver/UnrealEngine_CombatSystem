// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimationManager.generated.h"

/**
 * 
 */
enum class EMontageType
{
	NONE,
	FIST_ATTACK_MONTAGE,
	SWORD_ATTACK_MONTAGE,
	SPECIAL_ABILITY_MONTAGE,
	HIT_MONTAGE,
	FIST_SKILL_1_MONTAGE,
	MOVABLE_MONTAGE
};

extern EMontageType NameToMontageType(FName name);

UCLASS()
class ATTACK_API UAnimationManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	UAnimationManager();

	TMap<FName, UAnimMontage*> LoadAllMontages( ) { return _montages; }
	TMap<FName, UAnimationAsset*> LoadAllAnimations( ) { return _animations; }

private:

	TMap<FName, UAnimationAsset*> _animations;
	TMap<FName,UAnimMontage*> _montages;
};



UCLASS()
class ATTACK_API UCustomAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UCustomAnimNotify() {}

	void SetNotifyName(FName name) { _notifyName = name; }

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	void SetNotifyFunc(TFunction<void(AActor* owner)> func) { _notifyJob = func; }

	virtual FString GetNotifyName_Implementation() const;


private:
	TFunction<void(AActor* owner)> _notifyJob = [](AActor* owner) {};

	FName _notifyName;
};
