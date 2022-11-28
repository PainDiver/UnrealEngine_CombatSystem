// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Time/TimelinePack.h"
#include "../GlobalSystem/GlobalContext.h"
#include "../Time/TimerPack.h"
#include "Weapon.generated.h"

class UAttackGameInstance;
class AMainCharacter;
class UCharacterMovementComponent;
class UCharacterAnimInstance;
class UCombatComponent;

#define DECLARE_FUNC1(N,Behavior) Behavior##N(){};
#define DECLARE_FUNC2(N,Behavior) Behavior##N(){};DECLARE_FUNC1(1,Behavior)
#define DECLARE_FUNC3(N,Behavior) Behavior##N(){};DECLARE_FUNC2(2,Behavior)
#define DECLARE_FUNC4(N,Behavior) Behavior##N(){};DECLARE_FUNC3(3,Behavior)


enum
{
	SKILL_1 =0,
	SKILL_2,
	SKILL_3,
	SKILL_4,
	MAX_SKILL,
};

#define DECLARE_SKILL_FUNCTIONS(N) DECLARE_FUNC##N(N,virtual void Skill)


UENUM(BlueprintType, meta = (BlueprintSpawnableComponent))
enum class EWeaponType : uint8
{
	EWT_FIST,
	EWT_SWORD
};

UCLASS()
class ATTACK_API AWeapon : public AActor
{
	GENERATED_BODY()

	friend class UCombatComponent;

public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void						BeginPlay() override;

	DECLARE_SKILL_FUNCTIONS(4)

public:	
	// Called every frame
	virtual void						Tick(float DeltaTime) override;

	virtual void						Hit() {};

	virtual void						TakeDown() {};

	virtual void						EndTakeDown() {};

	virtual void						DamageEffect(AMainCharacter* target);

	void								Initialize();

	void								PerformSkill(int32 index);

	void								SetMontageName(FName name)		{ _montageName = name; }
	
	virtual void						Mount() {}

	TMap<FName, FTimelinePack>&			GetTimelinePackRef()			{ return _timelinePackages; }
	USkeletalMeshComponent*				GetSkeletalMesh()				{ return _mesh; }
	AMainCharacter*						GetTakeDownTarget()				{ return _takeDownTarget; }
	EWeaponType							GetWeaponType()					{ return _currentWeaponType; }
	FName								GetMontageName()				{ return _montageName; }


public:

	int32												_inventorySlotNumber = 0;

protected:

	TWeakObjectPtr<AMainCharacter>						_character;

	AMainCharacter*										_takeDownTarget;

	UCharacterMovementComponent*						_movementComponent;

	TWeakObjectPtr<UCombatComponent>					_combatComponent;

	UCharacterAnimInstance*								_animInstance;

	UAttackGameInstance*								_gameInstance;

	TStaticArray<TFunction<void()>,MAX_SKILL_NUM>		_skills;

	EWeaponType											_currentWeaponType;

	FName												_montageName = "";
	
	TMap<FName, FTimelinePack>							_timelinePackages;

	float												_damage = 50.f;
	float												_hitSlow = 0.2f;
	float												_freezeTime = 0.2f;
	bool												_timeDelayed = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* _mesh;

};




UCLASS()
class ATTACK_API AFist : public AWeapon
{
	GENERATED_BODY()

public:
	AFist();

public:

	virtual void			BeginPlay()override;

	virtual void			Hit()override;

	virtual void			TakeDown()override;
	
	virtual void			EndTakeDown()override;

	void					Mount()override;
	

	UFUNCTION()
	void					AccelerateTakeDown(float value);
	


	virtual void			Skill1()override;

private:
	FTimerPack				_timerPack;

	float					_skill_1_Damage = 100.f;

	UParticleSystemComponent* _tempTrail;
};


UCLASS()
class ATTACK_API ASword : public AWeapon
{
	GENERATED_BODY()

public:
	ASword();

public:

	virtual void			BeginPlay()override;

	virtual void			Tick(float deltaTIme)override;

	virtual void			Hit()override;

	virtual void			TakeDown()override;

	virtual void			EndTakeDown()override;
private:
};