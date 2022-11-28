// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterAnimInstance_BP.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../GlobalSystem/AttackGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "../Time/TimelinePack.h"
#include "Particles/ParticleSystemComponent.h"
#include "../FX/ParticleManager.h"

#define COMBAT_VALID_CHECK	if(!_movementComponent || !_character.IsValid() || !_currentWeapon){return;}
#define ANIM_VALID_CHECK	if(!_animInstance)return;
#define AIR_CHECK			if(_movementComponent->IsFalling())return;

#define COMBAT_VALID_CHECK_RETURN	if(!_movementComponent || !_character.IsValid()){return false;}
#define ANIM_VALID_CHECK_RETURN		if(!_animInstance)return false;


const TMap<FName /*CurveName*/, FName/*Callback name*/> TIMELINE_TYPE = {
	TPair<FName,FName>(FName("Roll"),FName("AccelerateRolling")),
};


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	_inventory.Reserve(INVENTORY_CAPACITY + 1);

}


void UCombatComponent::InitializeComponent()
{
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	_character = Cast<AMainCharacter>(GetOwner());
	if (_character.IsValid())
	{
		_animInstance = Cast<UCharacterAnimInstance>(_character->GetMesh()->GetAnimInstance());
		_movementComponent = _character->GetCharacterMovement();
		
		//RegisterWeapon, Add all weapon class name into arrow
		RegisterWeapon<AFist,ASword>();
		_currentWeapon = _inventory[1];

		float delta = GetWorld()->GetDeltaSeconds();

		for (auto& nameFunc : TIMELINE_TYPE)
		{
			UCurveFloat* curve = GAMEINSTANCE->GetCurveFloat(nameFunc.Key);
			if(curve)
				_timelinePackages.Add(nameFunc.Key, FTimelinePack(this,_character.Get(), curve, nameFunc.Value, delta));
		}

	}

}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::EquipWeapon(uint8 index)
{
	if (NO_ATTACKABLE_STATES.Find(_combatState) != INDEX_NONE)
	{
		return;
	}

	if (_inventory.Contains(index) && _currentWeapon != _inventory[index])
	{
		_animInstance->PlayMontageNormally("MovableMontage", "Swap",true);
		SetCombatState(ECombatState::ECS_SWAPING);
		FAttachmentTransformRules attachRule(EAttachmentRule::SnapToTarget, false);
		FString slotNum = FString::FromInt(_currentWeapon->_inventorySlotNumber);
		_currentWeapon->AttachToComponent(_character->GetMesh(), attachRule, FName("Slot" + slotNum));
		
		_currentWeapon = _inventory[index];		//if not fist
		if(_currentWeapon->GetSkeletalMesh())
			_currentWeapon->AttachToComponent(_character->GetMesh(), attachRule,FName("WeaponSocket"));
	}

}

void UCombatComponent::Attack()
{
	COMBAT_VALID_CHECK

	ANIM_VALID_CHECK

	AIR_CHECK

	if (NO_ATTACKABLE_STATES.Find(_combatState) != INDEX_NONE)
		return;

	SetCombatState(ECombatState::ECS_ATTACKING);
	StopState();

	if (!_animInstance->PlayAttackMontage(_currentWeapon->GetMontageName(), _combo))
	{
		return;
	}
	_movementComponent->StopMovementImmediately();
}

void UCombatComponent::RefreshCombo(float leftComboTime)
{
	_character->GetWorldTimerManager().ClearTimer(_comboTimer);
	_character->GetWorldTimerManager().SetTimer(_comboTimer, FTimerDelegate::CreateLambda(
		[&]()
		{
			EndCombo();
		}
	), leftComboTime+ (_currentWeapon->_timeDelayed ? _currentWeapon->_freezeTime : 0.f), false);
}

void UCombatComponent::EndCombo()
{
	if(GetCombatState()==ECombatState::ECS_ATTACKING)
		SetCombatState(ECombatState::ECS_NONE);

	_combo = 0;
	_character->bUseControllerRotationYaw = false;
	_movementComponent->bOrientRotationToMovement = true;
	_movementComponent->MaxWalkSpeed = _character->_defaultMaxWalkSpeed;
}


void UCombatComponent::Roll()
{
	COMBAT_VALID_CHECK

	ANIM_VALID_CHECK

	if (_movementComponent->IsFalling() || NO_DODGEABLE_STATES.Find(_combatState) != INDEX_NONE)
		return;

	SetCombatState(ECombatState::ECS_ROLLING);

	_movementComponent->bOrientRotationToMovement = true;
	FRotator rot = _movementComponent->GetLastInputVector().Rotation();
	_movementComponent->MaxWalkSpeed = 0.f;

	_character->SetActorRotation(_movementComponent->GetLastInputVector().Rotation());

	StartTimeline(_timelinePackages,"Roll");

	_animInstance->PlayMontageNormally(FName("SpecialAbilityMontage"), FName("Roll"),true);
}

void UCombatComponent::TakeDown()
{
	COMBAT_VALID_CHECK

	ANIM_VALID_CHECK

	if (NO_ATTACKABLE_STATES.Find(_combatState) != INDEX_NONE)
		return;

	SetCombatState(ECombatState::ECS_GRAPLING);
	StopState();

	_currentWeapon->TakeDown();
}

void UCombatComponent::Hit()
{
	COMBAT_VALID_CHECK
	_currentWeapon->Hit();
}

bool UCombatComponent::TakeDamage(ACharacter* damagingCharacter,float damage)
{
	COMBAT_VALID_CHECK_RETURN

	ANIM_VALID_CHECK_RETURN

	if (INVINCIBLE_STATES.Find(_combatState) != INDEX_NONE)
		return false;

	if(_combatState != ECombatState::ECS_GRAPLED)
		SetCombatState(ECombatState::ECS_TAKING_DAMAGE);
	
	StopState();

	
	//TODO
	//parring(damingCharacter)
	//damagingCharacter -> playaniminstance(missing punch, force)
	//SetState(missing punch)
	
	_character->SetHP(damage);

	int randNum = rand() % 3;
	FString num = FString::FromInt(randNum + 1);
	_animInstance->PlayMontageNormally(FName("HitMontage"), FName("Hit" + num));
	return true;
}

void UCombatComponent::Dodge()
{
	COMBAT_VALID_CHECK

	ANIM_VALID_CHECK


	if (NO_DODGEABLE_STATES.Find(_combatState) != INDEX_NONE)
	{
		return;
	}

	SetCombatState(ECombatState::ECS_DODGING);
	StopState();

	FString randDir = (rand() % 2 == 0) ? "Left" : "Right";
	_animInstance->PlayMontageNormally(FName("SpecialAbilityMontage"), FName("Dodge" + randDir),true);
}

void UCombatComponent::PerformSkill(int32 index)
{
	COMBAT_VALID_CHECK

	if (NO_SKILL_USABLE_STATES.Find(_combatState) != INDEX_NONE)
	{
		return;
	}
	SetCombatState(ECombatState::ECS_SKILL);
	_currentWeapon->PerformSkill(index);
}



void UCombatComponent::StopState()
{
	COMBAT_VALID_CHECK
	_movementComponent->MaxWalkSpeed = 0.f;
}

void UCombatComponent::InitializeState()
{
	COMBAT_VALID_CHECK
	SetCombatState(ECombatState::ECS_NONE);
	_movementComponent->MaxWalkSpeed = _character->_defaultMaxWalkSpeed;
}


/*
TIME LINE CALLBACK UFUNCTIONS
*/

void UCombatComponent::AccelerateRolling(float value)
{
	if(_character.IsValid())
		_character->LaunchCharacter(_character->GetActorForwardVector() * value, false, false);
}
