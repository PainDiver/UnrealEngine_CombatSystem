// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Animation/GeneralAnimInstance.h"
#include "../Weapon/Weapon.h"
#include "CharacterAnimInstance_BP.generated.h"

/**
 * 
 */
class AMainCharacter;
class UCharacterMovementComponent;
class UCombatComponent;
class UAttackGameInstance;


UCLASS()
class ATTACK_API UCharacterAnimInstance : public UGeneralAnimInstance
{
	GENERATED_BODY()

	struct FMontageComboSection
	{
		FMontageComboSection(FName sectionName, float minimumTimeForNextPose, float length, float start,float end) 
			:sectionName(sectionName), minimumTimeForNextPose(minimumTimeForNextPose), sectionLength(length), sectionStartAt(start),sectionEndAt(end) {}
		
		
		FName sectionName = "";
		float minimumTimeForNextPose = 0.f;
		float sectionLength = 0.f;
		float sectionStartAt = 0.f;
		float sectionEndAt = 0.f;
		TFunction<void()> delegate = []() {};
	};

public:
	UCharacterAnimInstance();

	virtual void				NativeInitializeAnimation()override;

	virtual void				NativeBeginPlay()override;

	virtual void				NativeUpdateAnimation(float DeltaTime)override;

	void						InitializeRef();

	void						InitializeWeaponAnim(UAnimMontage* montage,FName name);

	bool						PlayAttackMontage(FName montageName,uint32& combo);

	void						PushComboInfo(UAnimMontage* montage, TMap<uint32, FMontageComboSection>& comboSection,TFunction<void()> dele);



private:
	TWeakObjectPtr<AMainCharacter>						_character;

	UCharacterMovementComponent*						_characterMovement;

	TWeakObjectPtr<UCombatComponent>					_combatComponent;

	TMap<FName, TMap<uint32, FMontageComboSection>>		_comboSections;




	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta= (AllowPrivateAccess ="true"))
	float _speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector _velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector _velocityInCharacterSpace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool _isJumping = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool _bIsLocking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWeaponType _currentWeaponType = EWeaponType::EWT_FIST;
};
