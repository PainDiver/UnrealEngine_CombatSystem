// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerPack.h"
#include "../GlobalSystem/ComfyHeader.h"

void FTimerPack::PlayTimer(float deltaTime, float exitTime)
{
	owner->GetWorldTimerManager().ClearTimer(handle);
	timer = 0.f;
	bStop = false;

	startCallback();

	owner->GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda(
		[&,deltaTime,exitTime]()
		{
			if (!owner.IsValid())
				return;
			
			execCallback();
			if (timer > exitTime || bStop)
			{
				exitCallback();
				owner->GetWorldTimerManager().ClearTimer(handle);
				timer = 0.f;
			}
			timer += deltaTime;
		}), deltaTime, true);

}

void FTimerPack::StopTimer()
{
	bStop = true;
}

