// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "TimelinePack.generated.h"

/**
 * 
 */

USTRUCT()
struct ATTACK_API FTimelinePack
{
	GENERATED_BODY()

public:
	FTimelinePack() {}

	FTimelinePack(TWeakObjectPtr<UObject> obj,AActor* targetCharacter, UCurveFloat* curve, FName funcName, float delta, float limit = 3.f)
	{
		owner = obj;
		target = targetCharacter;
		curveFloat = curve;
		callbackFuncName = funcName;
		deltaTime = delta;
		limitTime = limit;
	}

	void SetOwner(UObject* obj) { owner = obj; }

	void SetCallback(FName name) { callbackFuncName = name; }

	void SetCurve(UCurveFloat* curve) { curveFloat = curve; }

	void SetDelta(float delta) { deltaTime = delta; }

	void SetLimit(float limit) { limitTime = limit; }

	void PlayTimeLine();

	void StopTimer();



private:
	TWeakObjectPtr<UObject>		owner;
	AActor*						target;

	FTimerHandle				timerHandle;
	FTimeline					timeline;
	UCurveFloat*				curveFloat;
	FName						callbackFuncName;
	float						deltaTime = 0.f;

	float						limitTime = 0.f;
	float						stopTimer = 0.f;
};

extern void StartTimeline(TMap<FName, FTimelinePack>& timelinePackages, FName name);
extern void StopTimeline(TMap<FName, FTimelinePack>& timelinePackages, FName name);
