// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectManager.h"

UObjectManager::UObjectManager()
{
}

void UObjectManager::DisableActor(AActor* actor)
{
	actor->SetActorTickEnabled(false);
	actor->SetActorHiddenInGame(true);
	actor->SetActorEnableCollision(false);
	actor->GetWorldTimerManager().ClearAllTimersForObject(actor);
}

void UObjectManager::EnableActor(AActor* actor)
{
	actor->SetActorTickEnabled(true);
	actor->SetActorHiddenInGame(false);
	actor->SetActorEnableCollision(true);
}
