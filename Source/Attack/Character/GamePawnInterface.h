// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GamePawnInterface.generated.h"


/**
 * 
 */
UINTERFACE(BlueprintType)
class ATTACK_API UGamePawnInterface : public UInterface
{
	GENERATED_BODY()
};


class ATTACK_API IGamePawnInterface
{
	GENERATED_BODY()

public:

	
	virtual void Attack() = 0;

	virtual bool TakeDamage(ACharacter* damagingCharacter,float damage) = 0;

};