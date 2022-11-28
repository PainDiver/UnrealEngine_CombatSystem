// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"


namespace RESOURCE
{
	const TMap<FName, const TCHAR*> CURVE_RESOURCE_PATHS = {
		TPair<FName,const TCHAR*>("Roll",TEXT("/Game/MyContent/Curve/Roll.Roll")),
		TPair<FName,const TCHAR*>("TakeDown",TEXT("/Game/MyContent/Curve/TakeDown.TakeDown")),
	};
};

UTimeManager::UTimeManager()
{
	for (auto& namePath : RESOURCE::CURVE_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UCurveFloat> curve(namePath.Value);
		if (curve.Succeeded())
		{
			_curveFloats.Add(namePath.Key, curve.Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Resource Not Found"), *namePath.Key.ToString());
		}
	}
}
