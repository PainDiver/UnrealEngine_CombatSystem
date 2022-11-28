// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Weapon/Weapon.h"
#include "../Time/TimelinePack.h"
#include "CombatComponent.generated.h"


class AMainCharacter;
class UCharacterAnimInstance;
class UCharacterMovementComponent;
class AWeapon;


const uint8 INVENTORY_CAPACITY =4;

enum class ECombatState
{
	ECS_NONE,
	ECS_ATTACKING,
	ECS_ROLLING,
	ECS_GRAPLING,
	ECS_TAKING_DAMAGE,
	ECS_DODGING,
	ECS_GRAPLED,
	ECS_SKILL,
	ECS_SWAPING,
};

const TArray<ECombatState> INVINCIBLE_STATES = { 
	ECombatState::ECS_DODGING,
	ECombatState::ECS_ROLLING,
	ECombatState::ECS_GRAPLING,
	ECombatState::ECS_GRAPLED,
	ECombatState::ECS_SKILL,
};


const TArray<ECombatState> NO_MOVABLE_STATES = {
	ECombatState::ECS_DODGING,
	ECombatState::ECS_GRAPLING,
	ECombatState::ECS_ROLLING,
	ECombatState::ECS_TAKING_DAMAGE,
	ECombatState::ECS_GRAPLED,
	ECombatState::ECS_SKILL,
};

const TArray<ECombatState> NO_ATTACKABLE_STATES = {
	ECombatState::ECS_DODGING,
	ECombatState::ECS_GRAPLING,
	ECombatState::ECS_ROLLING, 
	ECombatState::ECS_TAKING_DAMAGE,
	ECombatState::ECS_GRAPLED,
	ECombatState::ECS_SKILL,
	ECombatState::ECS_SWAPING
};

const TArray<ECombatState> NO_DODGEABLE_STATES = { 
	ECombatState::ECS_DODGING,
	ECombatState::ECS_GRAPLING,
	ECombatState::ECS_TAKING_DAMAGE,
	ECombatState::ECS_ROLLING,
	ECombatState::ECS_GRAPLED,
	ECombatState::ECS_SKILL,
};

const TArray<ECombatState> NO_SKILL_USABLE_STATES = {
	ECombatState::ECS_GRAPLING,
	ECombatState::ECS_TAKING_DAMAGE,
	ECombatState::ECS_ROLLING,
	ECombatState::ECS_GRAPLED,
	ECombatState::ECS_SKILL,
	ECombatState::ECS_SWAPING
};




UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ATTACK_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	static TStaticArray<FName, 2> TIMER_TYPE;

public:
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts

	virtual void					InitializeComponent()override;

	virtual void					BeginPlay() override;

public:
	// Called every frame
	virtual void					TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void							EquipWeapon(uint8 index);

	void							Attack();

	void							RefreshCombo(float leftComboTime);

	void							EndCombo();

	void							Roll();

	void							TakeDown();

	void							Hit();

	bool							TakeDamage(ACharacter* damagingCharacter,float damage);

	void							Dodge();

	void							PerformSkill(int32 index);
	
	void							StopState();

	void							InitializeState();

	UFUNCTION()
	void							AccelerateRolling(float value);


	ECombatState					GetCombatState() { return _combatState; }

	AWeapon*						GetCurrentWeapon() { return _currentWeapon; }

	TWeakObjectPtr<AMainCharacter>	GetCharacter() { return _character; }

	TMap<FName, FTimelinePack>&		GetTimelinePackRef(){ return _timelinePackages; }

	void SetCombatState(ECombatState ecs)
	{
		_combatState = ecs;
	}

	template <typename... WeaponTypes>
	typename TEnableIf<sizeof...(WeaponTypes) == 0/*void SIze*/, void>::type RegisterWeapon() {}

	template<typename WeaponType, typename... WeaponTypes>
	void RegisterWeapon()
	{
		FActorSpawnParameters params;
		params.Owner = _character.Get();

		if (TIsSame<WeaponType, AFist>::Value)
		{
			_currentWeapon = GetWorld()->SpawnActor<WeaponType>(params);
			if (!_currentWeapon)
				return;
			_inventory.Add(_inventory.Num() + 1, _currentWeapon);
			_currentWeapon->_inventorySlotNumber = _inventory.Num();

			_currentWeapon->SetMontageName("FistAttackMontage");
			_currentWeapon->_currentWeaponType = EWeaponType::EWT_FIST;
		}
		else if (TIsSame<WeaponType, ASword>::Value)
		{
			_currentWeapon = GetWorld()->SpawnActor<WeaponType>(params);
			if (!_currentWeapon)
				return;
			_inventory.Add(_inventory.Num() + 1, _currentWeapon);
			_currentWeapon->_inventorySlotNumber = _inventory.Num();

			_currentWeapon->SetMontageName("SwordAttackMontage");
			_currentWeapon->_currentWeaponType = EWeaponType::EWT_SWORD;

			FAttachmentTransformRules attachRule(EAttachmentRule::SnapToTarget, false);
			FString slotNum = FString::FromInt(_currentWeapon->_inventorySlotNumber);
			_currentWeapon->AttachToComponent(_character->GetMesh(), attachRule, FName("Slot" + slotNum));
		}

		RegisterWeapon<WeaponTypes...>();
	}



private:

	TWeakObjectPtr<AMainCharacter>		_character;

	UCharacterAnimInstance*				_animInstance;

	UCharacterMovementComponent*		_movementComponent;


	
	AWeapon*							_currentWeapon;


	TMap<uint32, AWeapon*>				_inventory;

	uint32								_combo = 0;

	FTimerHandle						_comboTimer;

	ECombatState						_combatState = ECombatState::ECS_NONE;

	TMap<FName, FTimelinePack>			_timelinePackages;
	
};
