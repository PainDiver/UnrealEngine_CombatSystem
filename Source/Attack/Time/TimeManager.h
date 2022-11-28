// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TimeManager.generated.h"

/**
 * 
 */

UCLASS()
class ATTACK_API UTimeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UTimeManager();

	UCurveFloat* GetCurve(FName name) { 
		if (_curveFloats.Contains(name))
		{
			return _curveFloats[name];
		}
		else
			return nullptr;
	}

private:


	TMap<FName, UCurveFloat*> _curveFloats;

};
