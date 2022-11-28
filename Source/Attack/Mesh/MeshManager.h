// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MeshManager.generated.h"

/**
 * 
 */

UCLASS()
class ATTACK_API UMeshManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMeshManager();


	USkeletalMesh* GetMesh(FName name) 
	{ 
		if (_skeletalMeshes.Contains(name))
			return _skeletalMeshes[name];
		else
		{
			if(GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("No Mesh"));
			return nullptr;
		}
	}

private:


	TMap<FName, USkeletalMesh*> _skeletalMeshes;

};
