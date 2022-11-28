// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

#include "../GlobalSystem/ComfyHeader.h"
#include "../GlobalSystem/AttackGameInstance.h"
#include "../Character/MainCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	_behaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Black Board Component"));

	_sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Perception Sesse"));
	

	_perceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	_perceptionComponent->ConfigureSense(*_sightConfig);
	SetPerceptionComponent(*_perceptionComponent);


	_sightConfig->SightRadius = _sightRadius;
	_sightConfig->LoseSightRadius = _loseSightRadius;
	_sightConfig->PeripheralVisionAngleDegrees = _fov;
	_sightConfig->SetMaxAge(_sightAge);

	_sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	_sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	_sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	_perceptionComponent->SetDominantSense(_sightConfig->GetSenseImplementation());
	_perceptionComponent->OnPerceptionUpdated.AddDynamic(this,&ThisClass::OnPawnDetected);
	
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!_behaviorTreeAsset)
	{
		_behaviorTreeAsset = GAMEINSTANCE->GetBehaviorTree("EnemyBehaviorTree");
	}

	//type check 
	if (_behaviorTreeAsset)
	{
		RunBehaviorTree(_behaviorTreeAsset);
		_behaviorTree->StartTree(*_behaviorTreeAsset);
		Blackboard->InitializeBlackboard(*_behaviorTreeAsset->GetBlackboardAsset());
	}
}

void AEnemyAIController::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

}

FRotator AEnemyAIController::GetControlRotation() const
{
	if (!GetPawn())
	{
		return FRotator();
	}
	
	return GetPawn()->GetActorRotation();
}

void AEnemyAIController::OnPawnDetected(const TArray<AActor*>& detactedPawns)
{
	for (auto actor : detactedPawns)
	{
		AMainCharacter* character = Cast<AMainCharacter>(actor);
		MoveToActor(character);
	}
}
