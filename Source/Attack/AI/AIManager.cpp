// Fill out your copyright notice in the Description page of Project Settings.


#include "AIManager.h"
#include "BehaviorTree/BehaviorTree.h"

namespace RESOURCE
{
	const TMap<FName, const TCHAR*> BEHAVIOR_TREE_RESOURCE_PATHS = {
		TPair<FName,const TCHAR*>("EnemyBehaviorTree", TEXT("/Game/MyContent/AI/BehaviorTree/TestBehaviorTree.TestBehaviorTree"))
	};
}

UAIManager::UAIManager()
{
	for (auto& namePath : RESOURCE::BEHAVIOR_TREE_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UBehaviorTree> behaviorTree(namePath.Value);
		if (behaviorTree.Succeeded())
		{
			_behaviorTrees.Add(namePath.Key, behaviorTree.Object);		
		}
		else
		{
		
		}
	}
}
