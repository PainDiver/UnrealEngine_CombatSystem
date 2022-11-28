// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MaterialManager.generated.h"

/**
 * 
 */

UCLASS()
class ATTACK_API UMaterialManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UMaterialManager();

	UMaterialInstance* GetMaterialInstance(FName name)
	{
		if (_materialInstances.Contains(name))
			return _materialInstances[name];
		else
			return nullptr;
	}


	UMaterial* GetMaterial(FName name) 
	{ 
		if (_materials.Contains(name))
			return _materials[name];
		else
			return nullptr;
	}


private:
	TMap<FName, UMaterial*>				_materials;
	TMap<FName, UMaterialInstance*>		_materialInstances;
};
