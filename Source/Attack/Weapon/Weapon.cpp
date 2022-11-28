// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../GlobalSystem/AttackGameInstance.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Character/GamePawnInterface.h"
#include "../Character/MainCharacter.h"
#include "../Character/CombatComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Character/CharacterAnimInstance_BP.h"
#include "Components/CapsuleComponent.h"


const TMap<FName /*CurveName*/, FName/*Callback name*/> TIMELINE_TYPE = {
	TPair<FName,FName>(FName("TakeDown"),FName("AccelerateTakeDown"))
};

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	RootComponent = _mesh;
	_skills = TStaticArray<TFunction<void()>, MAX_SKILL_NUM>(InPlace, []() {});
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::DamageEffect(AMainCharacter* target)
{
	_character->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array[0].Weight = 1.0;

	if (_gameInstance)
		_gameInstance->PlayParticleAtLocation("MuzzleFlash", target->GetActorLocation());

	FTimerHandle timerHandle;
	target->CustomTimeDilation = _hitSlow;
	target->GetWorldTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[=]()
		{
			_character->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array[0].Weight = 0.0f;
			target->CustomTimeDilation = 1.f;
		}), _freezeTime, false);
}

void AWeapon::Initialize()
{
	if(!_gameInstance)
		_gameInstance = GAMEINSTANCE;
	if (!_character.IsValid())
	{
		_character = Cast<AMainCharacter>(GetOwner());
		if (_character.IsValid())
		{
			_animInstance = _character->GetAnimInstance();
			_movementComponent = _character->GetCharacterMovement();
			_combatComponent = _character->GetCombatComponent();

			float delta = GetWorld()->GetDeltaSeconds();
			for (auto& nameFunc : TIMELINE_TYPE)
			{
				UCurveFloat* curve = GAMEINSTANCE->GetCurveFloat(nameFunc.Key);
				if (curve)
					_timelinePackages.Add(nameFunc.Key, FTimelinePack(this, _character.Get(), curve, nameFunc.Value, delta));
			}
		}
	}
}


void AWeapon::PerformSkill(int32 index)
{
	if (_skills[index])
		_skills[index]();
}

AFist::AFist(){}

void AFist::BeginPlay()
{
	Super::BeginPlay();

	_skills[SKILL_1] = [&]() {Skill1(); };
	
	//skill initialization
}

void AFist::Hit()
{
	Super::Hit();

	if (_gameInstance)
	{
		_gameInstance->PlaySoundAtLocation("AttackSound", _character->GetActorLocation());
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray; objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> actors; TArray<AActor*> ignoredActors; ignoredActors.Add(_character.Get());
	FVector start = _character->GetActorLocation() + _character->GetActorForwardVector() * 200.f;
	bool hit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), start, 150.f, objectTypesArray, nullptr, ignoredActors, actors);

	//DrawDebugSphere(GetWorld(), start, 150.f, 32, FColor::Red, false, 3.f);

	if (hit)
	{
		for (auto& actor : actors)
		{
			AMainCharacter* target = Cast<AMainCharacter>(actor);
			if (!target)
			{
				continue;
			}
			IGamePawnInterface* pawn = Cast<IGamePawnInterface>(actor);
			
			if (!pawn || !pawn->TakeDamage(_character.Get(), _damage))
				continue;

			FVector dir = target->GetActorLocation() - _character->GetActorLocation();
			dir.Normalize();
			_character->SetActorRotation(dir.Rotation());
			target->SetActorRotation((-dir).Rotation());
			target->LaunchCharacter(dir * 500.f, false, false);

			DamageEffect(target);
		}
	}
	else
	{
		return;
	}

	if (_gameInstance)
		_gameInstance->PlayCameraShake("AttackCameraShake");

	if (_character->GetCombatComponent()->GetCombatState() == ECombatState::ECS_GRAPLING)
	{
		return;
	}

	FTimerHandle timerHandle;
	_timeDelayed = true;
	_character->CustomTimeDilation = _hitSlow;
	_character->GetWorldTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[=]()
		{
			_character->CustomTimeDilation = 1.f;
			_timeDelayed = false;
		}), _freezeTime, false);
	

}

void AFist::TakeDown()
{
	Super::TakeDown();

	StartTimeline(_timelinePackages, "TakeDown");

	if(_character->GetLastMovementInputVector() != FVector::ZeroVector)
		_character->SetActorRotation(_character->GetLastMovementInputVector().Rotation());


	FHitResult hit;
	FVector start = _character->GetActorLocation();
	FVector end = start + _character->GetActorForwardVector() * 500.f;
	FCollisionQueryParams q;
	q.AddIgnoredActor(_character.Get());


	if (GetWorld())
		GetWorld()->LineTraceSingleByChannel(hit, start, end, ECollisionChannel::ECC_Pawn, q);

	if (hit.bBlockingHit)
	{
		AMainCharacter* target = Cast<AMainCharacter>(hit.GetActor());
		if (target)
		{
			_takeDownTarget = target;
			
			if (INVINCIBLE_STATES.Find(_takeDownTarget->GetCombatComponent()->GetCombatState()) != INDEX_NONE)
				goto FAIL;
			
			_takeDownTarget->GetCombatComponent()->StopState();
			_takeDownTarget->GetCombatComponent()->SetCombatState(ECombatState::ECS_GRAPLED);

			_character->GetAnimInstance()->PlayMontageNormally(FName("SpecialAbilityMontage"), FName("TakeDown"), true);
			_character->PlaySequencer();

			FVector dir = _character->GetActorLocation() - target->GetActorLocation();
			dir = FVector(dir.X, dir.Y, 0);

			_character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			target->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			
			target->SetActorRotation(dir.Rotation());
			target->SetActorLocation(_character->GetActorLocation() + _character->GetActorForwardVector() * 70.f);

			FAttachmentTransformRules attachRule(EAttachmentRule::KeepWorld, false);
			target->AttachToActor(_character.Get(), attachRule, FName("GrapleSocket"));

			UCharacterAnimInstance* instance = Cast<UCharacterAnimInstance>(_takeDownTarget->GetMesh()->GetAnimInstance());
			instance->PlayMontageNormally(FName("HitMontage"), FName("TakeDown_Hit"), true);
		
		}
	}
	else
	{
	FAIL:
		_character->GetAnimInstance()->PlayMontageNormally(FName("SpecialAbilityMontage"), FName("TakeDownFailed"));
	}

}

void AFist::EndTakeDown() 
{
	Super::EndTakeDown();
	if (!_combatComponent.IsValid() || !_character.IsValid())
		return;


	_combatComponent->InitializeState();
	_character->ExitSequencer();

	if (_takeDownTarget)
	{
		_character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		_takeDownTarget->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

		FDetachmentTransformRules rule(EDetachmentRule::KeepWorld, false);
		_takeDownTarget->DetachFromActor(rule);
		_takeDownTarget = nullptr;
	}
	
}

void AFist::Mount()
{
	AMainCharacter* target = Cast<AMainCharacter>(_takeDownTarget);
	if(target)
		DamageEffect(target);

	GAMEINSTANCE->PlayCameraShake("AttackCameraShake");

	FTimerHandle timerHandle;
	_timeDelayed = true;
	_character->CustomTimeDilation = _hitSlow;
	_character->GetWorldTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[=]()
		{
			_character->CustomTimeDilation = 1.f;
			_timeDelayed = false;
		}), _freezeTime, false);
}


void AFist::AccelerateTakeDown(float value)
{
	if (_character.IsValid())
		_character->LaunchCharacter(_character->GetActorForwardVector() * value, false, false);
}

void AFist::Skill1()
{
	if (!_character.IsValid())
	{
		return;
	}

	FVector lastVector = _character->GetLastMovementInputVector();
	if(lastVector != FVector::ZeroVector) 
		_character->SetActorRotation(lastVector.Rotation());


	_timerPack.owner =this;
	_character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	_timerPack.startCallback = [=]() {
		//GAMEINSTANCE->PlayParticleAtLocation("Dash",_character->GetActorLocation());
		if (!_tempTrail)
			_tempTrail = GAMEINSTANCE->PlayParticleAtMesh("DashTrail", _character->GetMesh());
		else
			_tempTrail->SetHiddenInGame(false);

		if (!_character.IsValid())
			return;

		_character->GetAnimInstance()->PlayMontageNormally("FistSkill_1", "SkillStart", true);
		_character->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array[0].Weight = 1.0; 
		_character->SetActorHiddenInGame(true);
		
		TArray<AActor*> actors;
		_character->GetAttachedActors(actors);
		for (auto& actor : actors)
		{
			actor->SetActorHiddenInGame(true);
		}
	};
	_timerPack.execCallback = 
		[&](){
			if (!_character.IsValid())
				return;

			_character->LaunchCharacter(_character->GetActorForwardVector() * 2000.f, false, false);

			TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray; objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			TArray<AActor*> actors; TArray<AActor*> ignoredActors; ignoredActors.Add(_character.Get());
			FVector start = _character->GetActorLocation() + _character->GetActorForwardVector() * 300.f;
			bool hit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), start, 250.f, objectTypesArray, nullptr, ignoredActors, actors);
			//DrawDebugSphere(GetWorld(), start, 250.f, 32, FColor::Red, false, 3.f);

			if (hit)
			{
				for (auto& actor : actors)
				{
					AMainCharacter* target =Cast<AMainCharacter>(actor);
					IGamePawnInterface* pawn = Cast<IGamePawnInterface>(actor);
					if (!pawn || !pawn->TakeDamage(_character.Get(), _skill_1_Damage))
						continue;

					DamageEffect(target);
					GAMEINSTANCE->PlayCameraShake("AttackCameraShake");
					_timerPack.StopTimer();
				}
			}
	};
	_timerPack.exitCallback = 
		[=]() {
			if (!_character.IsValid())
				return;

			_character->SetActorHiddenInGame(false);
			_character->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array[0].Weight = 0.f;
			_character->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			
			if (_tempTrail)
			{
				FTimerHandle handle;
				GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([=]() {_tempTrail->SetHiddenInGame(true); }), 0.7f, false);
			}
			TArray<AActor*> actors;
			for (auto& actor : actors)
			{
				actor->SetActorHiddenInGame(false);
			}		
		};


	_timerPack.PlayTimer(0.05f,0.1f);

}



ASword::ASword(){}

void ASword::BeginPlay()
{
	Super::BeginPlay();

	_mesh->SetSkeletalMesh(_gameInstance->GetMesh("SwordMesh"));
}

void ASword::Tick(float deltaTIme)
{
	Super::Tick(deltaTIme);

}


void ASword::Hit()
{
	Super::Hit();


	if (_gameInstance)
		_gameInstance->PlaySoundAtLocation("AttackSound", _character->GetActorLocation());


	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypesArray;
	objectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> actors; TArray<AActor*> ignoredActors;
	ignoredActors.Add(_character.Get());

	FVector start = _character->GetActorLocation() + _character->GetActorForwardVector() * 200.f;
	bool hit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), start, 150.f, objectTypesArray, nullptr, ignoredActors, actors);

	DrawDebugSphere(GetWorld(), start, 150.f, 32, FColor::Red, false, 3.f);

	if (hit)
	{
		for (auto& actor : actors)
		{
			AMainCharacter* target = Cast<AMainCharacter>(actor);
			if (!target)
			{
				continue;
			}
			IGamePawnInterface* pawn = Cast<IGamePawnInterface>(actor);

			if (!pawn || !pawn->TakeDamage(_character.Get(), _damage))
				continue;

			DamageEffect(target);
		}
	}
	else
	{
		return;
	}

	if (_gameInstance)
		_gameInstance->PlayCameraShake("AttackCameraShake");

	if (_character->GetCombatComponent()->GetCombatState() == ECombatState::ECS_GRAPLING)
	{
		return;
	}

	FTimerHandle timerHandle;
	_timeDelayed = true;
	_character->CustomTimeDilation = _hitSlow;
	_character->GetWorldTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[=]()
		{
			_character->CustomTimeDilation = 1.f;
			_timeDelayed = false;
		}), _freezeTime, false);
}


void ASword::TakeDown()
{
	Super::TakeDown();
}


void ASword::EndTakeDown()
{
	Super::EndTakeDown();
}