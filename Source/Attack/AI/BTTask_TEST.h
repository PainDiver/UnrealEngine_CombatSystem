// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_TEST.generated.h"

/**
 * 
 */
UCLASS()
class ATTACK_API UBTTask_TEST : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:

	UBTTask_TEST();

	virtual void OnGameplayTaskActivated(class UGameplayTask& task)override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;


};
