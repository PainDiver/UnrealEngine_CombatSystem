// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamePawnInterface.h"
#include "../GlobalSystem/GlobalContext.h"
#include "MainCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCharacterAnimInstance;
class UCombatComponent;
class UParticleComponent;
class USphereComponent;
class AAIController;

UCLASS()
class ATTACK_API AMainCharacter : public ACharacter, public IGamePawnInterface
{
	GENERATED_BODY()

	friend class UCombatComponent;

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void				BeginPlay() override;

public:	
	// Called every frame
	virtual void				Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void				SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void						MoveForward(float value);

	void						MoveRight(float value);

	void						LookUp(float value);

	void						Turn(float value);

	void						Jump();

	virtual void				Attack()override;

	virtual bool				TakeDamage(ACharacter* damagingCharacter, float damage)override;

	virtual void				Landed(const FHitResult& Hit)override;

	void						Roll();

	void						TakeDown();

	void						Dodge();

	void						PerformSlot1();

	void						Swap(const int slot);

	void						LockOn();

	void						LockOff();

	bool						IsLockingOn() { return _lockOnTarget!=nullptr ? true : false; }

	const USkeletalMeshSocket*	GetSocket(FName name);

	UCharacterAnimInstance*		GetAnimInstance();

	UCameraComponent*			GetCameraComponent() { return _camera; }

	UFUNCTION(BlueprintCallable)
	UCombatComponent*			GetCombatComponent() { return _combatComponent; }


	UFUNCTION(BlueprintNativeEvent)
	void						PlaySequencer();
	virtual void				PlaySequencer_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void						ExitSequencer();
	virtual void				ExitSequencer_Implementation();

	void SetHP(uint32 damage)
	{
		_hp = FMath::Clamp<uint32>(_hp - damage, 0, _hp);
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("%d"), _hp));

		if (_hp <= 0)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Dead"));
	}


public:

	//FOR AI TEST
	AAIController*										_aiController;

private:

	TStaticArray<TFunction<void()>,MAX_SKILL_NUM>		_performSkills;

	UCharacterMovementComponent*						_movementComponent;

	UCombatComponent*									_combatComponent;

	UParticleComponent*									_particleComponent;

	UCharacterAnimInstance*								_animInstance;

	APlayerController*									_controller;
	
	TWeakObjectPtr<AMainCharacter>						_lockOnTarget;
	

	uint32												_hp = 1000.f;
	float												_defaultMaxWalkSpeed = 600.f;
	float												_defaultJumpZVelocity = 400.f;
	uint32												_additionalJumpCount =1;




	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* _camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* _sequenceCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* _springArm;
};
