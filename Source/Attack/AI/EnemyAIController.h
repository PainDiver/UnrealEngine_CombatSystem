// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class ATTACK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AEnemyAIController();

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn)override;

	virtual void Tick(float deltaSeconds)override;

	virtual FRotator GetControlRotation()const override;

	UFUNCTION()
	void OnPawnDetected(const TArray<AActor*>& detactedPawns);

	float _sightRadius = 500.f;
	float _sightAge = 5.f;
	float _loseSightRadius = _sightRadius + 50.f;
	float _fov = 90.f;

private:

	UAISenseConfig_Sight* _sightConfig;
	UAIPerceptionComponent* _perceptionComponent;
	UBehaviorTreeComponent* _behaviorTree;


	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta =(AllowPrivateAccess = "true"))
	UBehaviorTree* _behaviorTreeAsset;	
};
