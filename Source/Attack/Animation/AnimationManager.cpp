// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationManager.h"

namespace RESOURCE
{
	const TMap<FName, const TCHAR*> MONTAGE_RESOURCE_PATHS = {
		TPair<FName,const TCHAR*>("SwordAttackMontage",TEXT("/Game/MyContent/Hero/Animation/Sword_Attack_Montage.Sword_Attack_Montage")),
		TPair<FName,const TCHAR*>("FistAttackMontage",TEXT("/Game/MyContent/Hero/Animation/Fist_Attack_Montage.Fist_Attack_Montage")),
		TPair<FName,const TCHAR*>("SpecialAbilityMontage",TEXT("/Game/MyContent/Hero/Animation/SpecialAbility_Montage.SpecialAbility_Montage")),
		TPair<FName,const TCHAR*>("HitMontage",TEXT("/Game/MyContent/Hero/Animation/HitReactions_Montage.HitReactions_Montage")),
		TPair<FName,const TCHAR*>("FistSkill_1",TEXT("/Game/MyContent/Hero/Animation/FistSkill_1.FistSkill_1")),
		TPair<FName,const TCHAR*>("MovableMontage",TEXT("/Game/MyContent/Hero/Animation/Movable_Montage.Movable_Montage")),
	};

	const TMap<FName, const TCHAR*> ANIMATION_RESOURCE_PATHS = {
	};

};

EMontageType NameToMontageType(FName name)
{
	if (name == "FistAttackMontage") return EMontageType::FIST_ATTACK_MONTAGE;
	else if (name == "SwordAttackMontage") return EMontageType::SWORD_ATTACK_MONTAGE;
	else if (name == "SpecialAbilityMontage") return EMontageType::SPECIAL_ABILITY_MONTAGE;
	else if (name == "HitMontage") return EMontageType::HIT_MONTAGE;
	else if (name == "FistSkill_1") return EMontageType::FIST_SKILL_1_MONTAGE;
	else if (name == "MovableMontage") return EMontageType::MOVABLE_MONTAGE;
	return EMontageType::NONE;
}


UAnimationManager::UAnimationManager()
{
	
	for (auto& namePath : RESOURCE::MONTAGE_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UAnimMontage> montage(namePath.Value);
		if (montage.Succeeded())
		{
			_montages.Add(namePath.Key, montage.Object);
		}
		else
		{
			// failed to load montage
		}
	}

	for (auto& namePath : RESOURCE::ANIMATION_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UAnimationAsset> animation(namePath.Value);
		if (animation.Succeeded())
		{
			_animations.Add(namePath.Key, animation.Object);
		}
		else
		{
			// failed to load montage
		}
	}

}

void UCustomAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* actor = MeshComp->GetOwner();
	_notifyJob(actor);
}

FString UCustomAnimNotify::GetNotifyName_Implementation() const
{
	FString string = Super::GetNotifyName_Implementation();
	string = FString(_notifyName.ToString());
	return string;
}
