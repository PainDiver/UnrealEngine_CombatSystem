// Fill out your copyright notice in the Description page of Project Settings.


#include "TimelinePack.h"
#include "../GlobalSystem/ComfyHeader.h"

void FTimelinePack::PlayTimeLine()
{
	if (!owner.IsValid() || !target)
		return;

	FOnTimelineFloat callback;
	callback.BindUFunction(owner.Get(), callbackFuncName);
	
	timeline.AddInterpFloat(curveFloat,callback);
	timeline.PlayFromStart();

	target->GetWorldTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[&]() {

			if ( (limitTime != 0.f && limitTime <= stopTimer) || !timeline.IsPlaying())
			{
				if (target)
					target->GetWorldTimerManager().ClearTimer(timerHandle);
				timeline.Stop();
				stopTimer = 0.f;
			}

			timeline.TickTimeline(stopTimer);
			stopTimer += 0.1f;
		}),0.1f, true);
}

void FTimelinePack::StopTimer()
{
	timeline.Stop();
}

void StartTimeline(TMap<FName, FTimelinePack>& timelinePackages, FName name)
{
	COMFYDEBUG("good");

	if (timelinePackages.Contains(name))
		timelinePackages[name].PlayTimeLine();
}

void StopTimeline(TMap<FName, FTimelinePack>& timelinePackages, FName name)
{
	if (timelinePackages.Contains(name))
		timelinePackages[name].StopTimer();
}