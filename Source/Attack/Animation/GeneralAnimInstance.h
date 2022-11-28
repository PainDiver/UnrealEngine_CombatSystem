// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GeneralAnimInstance.generated.h"

/**
 * 
 */


UCLASS()
class ATTACK_API UGeneralAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation()override;


	bool PushNotifyAt(FAnimNotifyEvent& notifyEvent, FName notifyName, TFunction<void(AActor* owner)> notifyJob);

	void PlayMontageNormally(FName montage, FName section, bool bForce= false);


	TMap<FName,UAnimationAsset*> LoadAnimation();

	TMap<FName,UAnimMontage*> LoadMontage();

protected:

	TMap<FName, UAnimMontage*> _montages;

	TMap<FName, UAnimationAsset*> _animations;

};
