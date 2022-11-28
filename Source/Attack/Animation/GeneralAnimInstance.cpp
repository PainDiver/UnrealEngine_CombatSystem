// Fill out your copyright notice in the Description page of Project Settings.


#include "GeneralAnimInstance.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "AnimationManager.h"
#include "../GlobalSystem/AttackGameInstance.h"


void UGeneralAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

bool UGeneralAnimInstance::PushNotifyAt(FAnimNotifyEvent& notifyEvent, FName notifyName, TFunction<void(AActor* owner)> notifyJob)
{
	if (notifyEvent.NotifyName == notifyName)
	{

		UCustomAnimNotify* notify = NewObject<UCustomAnimNotify>();
		if (!notify)
			return false;

		notify->SetNotifyFunc([=](AActor* owner)
			{
				notifyJob(owner);
			});
		notify->SetNotifyName(notifyName);

		notifyEvent.Notify = Cast<UAnimNotify>(notify);

		return true;
	}
	return false;
}

void UGeneralAnimInstance::PlayMontageNormally(FName montageName, FName section, bool bForce)
{
	if (!_montages.Contains(montageName))
	{
		return;
	}

	UAnimMontage* montage = _montages[montageName];

	if (!Montage_IsPlaying(montage) || (IsAnyMontagePlaying() && bForce))
	{
		Montage_Play(montage);
		Montage_JumpToSection(section);
	}
}

TMap<FName, UAnimationAsset*> UGeneralAnimInstance::LoadAnimation()
{
	return ANIMATION_MANAGER->LoadAllAnimations();
}

TMap<FName, UAnimMontage*> UGeneralAnimInstance::LoadMontage()
{
	return ANIMATION_MANAGER->LoadAllMontages();
}
