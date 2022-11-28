// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance_BP.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/MainCharacter.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "CombatComponent.h"
#include "../GlobalSystem/AttackGameInstance.h"
#include "../Animation/AnimationManager.h"


#define GET_COMBATCOMPONENT(owner) 			AMainCharacter* character = Cast<AMainCharacter>(owner); \
											if (!character)return;									 \
											UCombatComponent* combatComponent = character->GetCombatComponent();\
											combatComponent\


UCharacterAnimInstance::UCharacterAnimInstance()
{
}

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	_animations = LoadAnimation();
	_montages = LoadMontage();

	InitializeRef();

	for (auto& montage : _montages)
	{
		EMontageType type = NameToMontageType(montage.Key);

		switch (type)
		{
			case EMontageType::FIST_ATTACK_MONTAGE:
			case EMontageType::SWORD_ATTACK_MONTAGE:
				InitializeWeaponAnim(montage.Value,montage.Key);
				break;
			
			case EMontageType::SPECIAL_ABILITY_MONTAGE:
				for (auto& notifyEvent : montage.Value->Notifies)
				{
					PushNotifyAt(notifyEvent, "EndRoll", [=](AActor* owner) { GET_COMBATCOMPONENT(owner);StopTimeline(character->GetCombatComponent()->GetTimelinePackRef(), "Roll"); });
					PushNotifyAt(notifyEvent, "Hit",
						[=](AActor* owner) {
							GET_COMBATCOMPONENT(owner)->Hit();
							FVector loc = character->GetActorLocation() - FVector(0, 0, character->GetDefaultHalfHeight());
							GAMEINSTANCE->PlayParticleAtLocation(FName("RockBurst"), loc, FVector(0.4f));
						});
					PushNotifyAt(notifyEvent, "Mount", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->GetCurrentWeapon()->Mount(); });

					PushNotifyAt(notifyEvent, "FinishRoll", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
					PushNotifyAt(notifyEvent, "EndGrabbing", [=](AActor* owner) {GET_COMBATCOMPONENT(owner); StopTimeline(character->GetCombatComponent()->GetCurrentWeapon()->GetTimelinePackRef(), "TakeDown"); });
					PushNotifyAt(notifyEvent, "EndTakeDown", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->GetCurrentWeapon()->EndTakeDown(); });
					PushNotifyAt(notifyEvent, "EndDodge", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
				}
				break;
			case EMontageType::HIT_MONTAGE:
				for (auto& notifyEvent : montage.Value->Notifies)
				{
					PushNotifyAt(notifyEvent, "EndTakeDamage", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
					PushNotifyAt(notifyEvent, "EndTakeDownHit", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
				}
				break;

			case EMontageType::FIST_SKILL_1_MONTAGE:
				for (auto& notifyEvent : montage.Value->Notifies)
				{
					PushNotifyAt(notifyEvent, "EndSkill", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
				}
				break;
			case EMontageType::MOVABLE_MONTAGE:
				for (auto& notifyEvent : montage.Value->Notifies)
				{
					PushNotifyAt(notifyEvent, "EndSwap", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->InitializeState(); });
				}
				break;

		}

	}
}


void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	InitializeRef();
}

void UCharacterAnimInstance::InitializeRef()
{

	_character = Cast<AMainCharacter>(TryGetPawnOwner());
	if (_character.IsValid() && _characterMovement && _combatComponent.IsValid())
	{
		_speed = _characterMovement->Velocity.Length();
		_velocity = _characterMovement->Velocity;
		_isJumping = _characterMovement->IsFalling();
		if (_combatComponent.Get()->GetCurrentWeapon())
		{
			_currentWeaponType = _combatComponent->GetCurrentWeapon()->GetWeaponType();
		}
		
		if (_character->IsLockingOn())
		{
			_bIsLocking = true;
			_velocityInCharacterSpace = _character->GetTransform().InverseTransformVector(_character->GetLastMovementInputVector() * _speed);
		}
		else
		{
			_bIsLocking = false;
		}
	}
	else if (_character.IsValid())
	{
		_characterMovement = _character->GetCharacterMovement();
		_combatComponent = _character->GetCombatComponent();
	}

}

void UCharacterAnimInstance::InitializeWeaponAnim(UAnimMontage* montage,FName name)
{
	TMap<uint32, FMontageComboSection> comboSection;
	PushComboInfo(montage, comboSection, [=]() {if (_character.IsValid()) { _character->LaunchCharacter(500 * _character->GetActorForwardVector(), false, false); }});
	_comboSections.Add(name,comboSection);

	uint32 index = 0;
	for (auto& notifyEvent : montage->Notifies)
	{
		if (PushNotifyAt(notifyEvent, "combo", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->RefreshCombo(_comboSections[name][index + 1].sectionLength);}))
		{
			index++;
		}
		PushNotifyAt(notifyEvent, "Hit", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->Hit(); });
		PushNotifyAt(notifyEvent, "EndCombo", [=](AActor* owner) {GET_COMBATCOMPONENT(owner)->EndCombo(); });
	}
}



bool UCharacterAnimInstance::PlayAttackMontage(FName montageName,uint32& combo)
{
	if (!_characterMovement || !_character.IsValid() || !_combatComponent.IsValid()) { return false; }

	if (!_montages.Contains(montageName) || !_comboSections.Contains(montageName) || combo >= static_cast<uint32>(_comboSections[montageName].Num()))
		return false;


	UAnimMontage* montage = _montages[montageName];
	FMontageComboSection section = _comboSections[montageName][combo];

	if (_comboSections[montageName][combo].sectionName == "End")
	{
		FTimerHandle handle;
		_character->GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda(
			[=]() 
			{
				_combatComponent->EndCombo();
			}
		), 0.5f, false);
		combo++;
		return false;
	}

	if (combo ==0)
	{
		Montage_Play(montage);
		Montage_JumpToSection(section.sectionName);
		section.delegate();
		combo++;
		return true;
	}
	else if(_comboSections[montageName][combo-1].minimumTimeForNextPose <= Montage_GetPosition(montage) || !Montage_IsPlaying(montage))
	{
		Montage_Play(montage);
		Montage_JumpToSection(section.sectionName);
		section.delegate();
		combo++;
		return true;
	}
	

	return false;
}


void UCharacterAnimInstance::PushComboInfo(UAnimMontage* montage, TMap<uint32, FMontageComboSection>& comboSections , TFunction<void()> dele)
{
	FName sectionName;
	uint16 index = 0;
	uint16 notifyIndex = 0;
	TArray<FAnimNotifyEvent> notifes = montage->Notifies;

	while (true)
	{
		sectionName = montage->GetSectionName(index);
		if (sectionName == "None")
			break;

		float comboLimitTime = 0.f;
		float sectionLength = montage->GetSectionLength(index);
		float sectionStartAt=0.f;
		float sectionEndAt=0.f;
		montage->GetSectionStartAndEndTime(index, sectionStartAt, sectionEndAt);
		while (true)
		{
			if (notifes.Num() <= notifyIndex)
				break;

			if (notifes[notifyIndex].NotifyName != "combo")
			{
				notifyIndex++;
			}
			else
			{

				comboLimitTime = notifes[notifyIndex].GetTime();
				notifyIndex++;
				break;
			}
		}

		FMontageComboSection comboSection = { sectionName, comboLimitTime,sectionLength,sectionStartAt,sectionEndAt};
		comboSections.Add(index, comboSection);
		comboSections[index].delegate = dele;

		++index;
	}

	// tail
	FMontageComboSection comboSection{ FName("End"),0.f,0.f,0.f,0.f };
	comboSections.Add(index, comboSection);
}
