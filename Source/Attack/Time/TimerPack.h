// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimerPack.generated.h"
/**
 * 
 */

USTRUCT()
struct ATTACK_API FTimerPack
{
	GENERATED_BODY()

	FTimerPack() {}

	FTimerPack(AActor* TimerOwner, TFunction<void()> start = []() {}, TFunction<void()> exec = []() {}, TFunction<void()> exit = []() {}) :
		owner(TimerOwner),startCallback(start),execCallback(exec), exitCallback(exit) {}

	void PlayTimer(float deltaTime, float exitTime);
	
	void StopTimer();


	TWeakObjectPtr<AActor> owner;
	TFunction<void()> startCallback;
	TFunction<void()> execCallback;
	TFunction<void()> exitCallback;
	FTimerHandle handle;
	float timer = 0.f;

	bool bStop = false;
};
