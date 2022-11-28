// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterAnimInstance_BP.h"
#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AIController.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "../GlobalSystem/AttackGameInstance.h"
#include "../AI/EnemyAIController.h"


#define COMBAT_VALID_CHECK	if(!_combatComponent || !_movementComponent){return;}
#define ANIM_VALID_CHECK	if(!_animInstance)return;

#define COMBAT_VALID_CHECK_RETURN if(!_combatComponent || !_movementComponent){return false;}

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_springArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");

	_camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	_camera->SetupAttachment(_springArm);

	_springArm->SetupAttachment(RootComponent);
	_springArm->SocketOffset.X = -150.f; _springArm->SocketOffset.Z = 60.f;
	_springArm->bUsePawnControlRotation = true;

	_sequenceCamera = CreateDefaultSubobject<UCameraComponent>("SequenceCamera");
	_sequenceCamera->SetupAttachment(RootComponent);

	FVector cameraLoc = FVector(-_springArm->TargetArmLength+ -150.f , 0.f, 60.f);
	_sequenceCamera->SetWorldLocation(cameraLoc);


	_movementComponent = GetCharacterMovement();

	_movementComponent->JumpZVelocity = _defaultJumpZVelocity;
	_movementComponent->bOrientRotationToMovement = true;
	_movementComponent->MaxWalkSpeed = _defaultMaxWalkSpeed;

	_combatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	

	_camera->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
	
	_performSkills = TStaticArray<TFunction<void()>, MAX_SKILL_NUM>(EInPlace::InPlace,
		[=]() {if (_combatComponent) _combatComponent->PerformSkill(0);});


	AIControllerClass = AEnemyAIController::StaticClass();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	FWeightedBlendable postProcess(0.0, GAMEINSTANCE->GetMaterialInstance("RadialBlurMaterial"));
	_camera->PostProcessSettings.WeightedBlendables.Array.Add(postProcess);

	_animInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	_controller = GetWorld()->GetFirstPlayerController();

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (_lockOnTarget.IsValid())
	{
		FVector dir = _lockOnTarget->GetActorLocation() - GetActorLocation();
		FRotator rot = FMath::RInterpTo(GetActorForwardVector().Rotation(), dir.Rotation(),DeltaTime,10.f);
		rot = FRotator(0, rot.Yaw, 0);
		SetActorRotation(rot);
		_movementComponent->MaxWalkSpeed = 200.f;
		_movementComponent->bOrientRotationToMovement = false;
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMainCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMainCharacter::Turn);


	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Released, this, &ThisClass::StopJumping);

	PlayerInputComponent->BindAction(FName("Roll"), EInputEvent::IE_Pressed, this, &ThisClass::Roll);

	PlayerInputComponent->BindAction(FName("Attack"), EInputEvent::IE_Pressed, this, &ThisClass::Attack);

	PlayerInputComponent->BindAction(FName("TakeDown"), EInputEvent::IE_Pressed, this, &ThisClass::TakeDown);

	PlayerInputComponent->BindAction(FName("Slot1"), EInputEvent::IE_Pressed, this, &ThisClass::PerformSlot1);

	PlayerInputComponent->BindAction(FName("LockOn"), EInputEvent::IE_Pressed, this, &ThisClass::LockOn);
	PlayerInputComponent->BindAction(FName("LockOn"), EInputEvent::IE_Released, this, &ThisClass::LockOff);

	//PlayerInputComponent->BindAction(FName("Dodge"), EInputEvent::IE_Pressed, this, &ThisClass::Dodge);

	DECLARE_DELEGATE_OneParam(FSwapDelegate, const int);
	PlayerInputComponent->BindAction<FSwapDelegate>(FName("Swap1"), EInputEvent::IE_Pressed, this, &ThisClass::Swap,1);
	PlayerInputComponent->BindAction<FSwapDelegate>(FName("Swap2"), EInputEvent::IE_Pressed, this, &ThisClass::Swap,2);
	PlayerInputComponent->BindAction<FSwapDelegate>(FName("Swap3"), EInputEvent::IE_Pressed, this, &ThisClass::Swap,3);
	PlayerInputComponent->BindAction<FSwapDelegate>(FName("Swap4"), EInputEvent::IE_Pressed, this, &ThisClass::Swap,4);

}


void AMainCharacter::MoveForward(float value)
{
	COMBAT_VALID_CHECK

	if (NO_MOVABLE_STATES.Find(_combatComponent->GetCombatState()) != INDEX_NONE)
		return;

	FRotator rot(0.f, GetControlRotation().Yaw, 0.f);
	FVector dir = FRotationMatrix(rot).GetUnitAxis(EAxis::X);
	
	AddMovementInput(dir, value);

}

void AMainCharacter::MoveRight(float value)
{
	COMBAT_VALID_CHECK

	if (NO_MOVABLE_STATES.Find(_combatComponent->GetCombatState()) !=INDEX_NONE)
		return;

	FRotator rot(0.f, GetControlRotation().Yaw, 0.f);
	FVector dir = FRotationMatrix(rot).GetUnitAxis(EAxis::Y);

	AddMovementInput(dir, value);
}

void AMainCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AMainCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AMainCharacter::Jump()
{
	COMBAT_VALID_CHECK
	ANIM_VALID_CHECK
	
	if (_combatComponent->GetCombatState() != ECombatState::ECS_NONE)
		return;

	if (!_movementComponent->IsFalling())
	{
		_movementComponent->bOrientRotationToMovement = false;
		ACharacter::Jump();
	}
	else if(_additionalJumpCount>0)
	{
		_additionalJumpCount--;
		_animInstance->PlayMontageNormally(FName("SpecialAbilityMontage"), FName("DoubleJump"));
		LaunchCharacter(FVector::UpVector * 600.f, false,false);
		_movementComponent->bOrientRotationToMovement = false;
	}
}

void AMainCharacter::Attack()
{
	COMBAT_VALID_CHECK

	_combatComponent->Attack();
}

bool AMainCharacter::TakeDamage(ACharacter* damagingCharacter,float damage)
{
	COMBAT_VALID_CHECK_RETURN
	if (damagingCharacter == nullptr)
		return false;

	return _combatComponent->TakeDamage(damagingCharacter,damage);
}

void AMainCharacter::Landed(const FHitResult& Hit)
{
	_movementComponent->bOrientRotationToMovement = true;
	_additionalJumpCount = DEFAULT_ADDITIONAL_JUMP_COUNT;

	//TODO make foot step sound here
}

void AMainCharacter::Roll()
{
	COMBAT_VALID_CHECK
	_combatComponent->Roll();
}


void AMainCharacter::TakeDown()
{
	COMBAT_VALID_CHECK
	_combatComponent->TakeDown();
}

void AMainCharacter::Dodge()
{
	COMBAT_VALID_CHECK
	_combatComponent->Dodge();
}

void AMainCharacter::PerformSlot1()
{
	COMBAT_VALID_CHECK
	_performSkills[0]();
}

void AMainCharacter::Swap(const int slot)
{
	COMBAT_VALID_CHECK
	_combatComponent->EquipWeapon(slot);
}

void AMainCharacter::LockOn()
{
	FVector start;
	TArray<TEnumAsByte<EObjectTypeQuery>> actorTypes; actorTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TArray<AActor*> ignored; ignored.Add(this);
	TArray<AActor*> actors;
	bool success = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), 800.f, actorTypes, nullptr, ignored, actors);

	if (success)
	{
		double minAngle = MAX_FLT;
		for (auto& actor : actors)
		{
			AMainCharacter* target = Cast<AMainCharacter>(actor);
			if (!target)
				continue;

			FVector dirToTarget = actor->GetActorLocation() - GetControlRotation().Vector();
			dirToTarget.Normalize();
			double cos = FVector::DotProduct(dirToTarget, GetControlRotation().Vector());
			double angle = FMath::Acos(cos);

			if (abs(angle) < minAngle)
			{
				_lockOnTarget = target;
				minAngle = angle;
			}
		}
	}
}

void AMainCharacter::LockOff()
{
	bUseControllerRotationYaw = false;
	_movementComponent->bOrientRotationToMovement = true;
	_lockOnTarget = nullptr;
	_movementComponent->MaxWalkSpeed = _defaultMaxWalkSpeed;
}



void AMainCharacter::PlaySequencer_Implementation()
{
	
}

void AMainCharacter::ExitSequencer_Implementation()
{

}

const USkeletalMeshSocket* AMainCharacter::GetSocket(FName name)
{
	return GetMesh()->GetSocketByName(name);
}

UCharacterAnimInstance* AMainCharacter::GetAnimInstance()
{
	return Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

