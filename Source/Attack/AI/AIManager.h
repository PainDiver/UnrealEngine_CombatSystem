// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AIManager.generated.h"

/**
 * 
 */

class UBehaviorTree;

UCLASS()
class ATTACK_API UAIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UAIManager();

	UBehaviorTree* GetBehaviorTree(FName name) {
		if (_behaviorTrees.Contains(name))
			return _behaviorTrees[name];
		else
			return nullptr;
	}


private:
	TMap<FName, UBehaviorTree*> _behaviorTrees;
};
