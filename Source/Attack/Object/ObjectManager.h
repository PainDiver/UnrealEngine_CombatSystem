// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/World.h"
#include "../GlobalSystem/ComfyHeader.h"
#include "GameFramework/Character.h"
#include "ObjectManager.generated.h"

/**
 * 
 */
UCLASS()
class ATTACK_API UObjectManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	UObjectManager();

	AActor* SpawnPoolActorByOrigin(FName name, TSubclassOf<AActor> type, const FVector& Loc = FVector::ZeroVector, const FRotator& Rot = FRotator::ZeroRotator,const FVector& scale = FVector::OneVector)
	{

		if (!_objectPools.Contains(name))
			_objectPools.Add(name, TArray<AActor*>());

		for (auto object : _objectPools[name])
		{
			if (object->IsHidden() && !object->IsActorTickEnabled())
			{
				object->DispatchBeginPlay();
				EnableActor(object);
				object->SetActorLocation(Loc);
				object->SetActorRotation(Rot);
				
				TArray<AActor*> children;
				object->GetAttachedActors(children);
				for (auto child : children)
				{
					EnableActor(child);
				}

				return object;
			}
		}

		AActor* actor = GetWorld()->SpawnActor(type);
		COMFYDEBUG("%s", *name.ToString());

		if (actor)
		{
			actor->SetActorLocation(Loc);
			actor->SetActorRotation(Rot);
			actor->SetActorScale3D(scale);
			
			// in case of character
			ACharacter* character = Cast<ACharacter>(actor);
			if (character)
			{
				character->SpawnDefaultController();
			}
			return actor;
		}
		else
		{
			return nullptr;
		}

	}



	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T, AActor>::Value,T*>::type SpawnPoolActor(const FVector& Loc = FVector::ZeroVector,const FRotator& Rot =FRotator::ZeroRotator, const FVector& scale = FVector::OneVector)
	{
		if (sizeof(T) == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No size"));
			return nullptr;
		}
		
		FName name = T::StaticClass()->GetFName();
		
		if (!_objectPools.Contains(name))
			_objectPools.Add(name, TArray<AActor*>());

		for (auto object : _objectPools[name])
		{
			T* obj = Cast<T>(object);
			if (obj->IsHidden() && !obj->IsActorTickEnabled())
			{
				obj->DispatchBeginPlay();
				EnableActor(obj);
				obj->SetActorLocation(Loc);
				obj->SetActorRotation(Rot);
				

				TArray<AActor*> children;
				obj->GetAttachedActors(children);
				for (auto child : children)
				{
					EnableActor(child);
				}

				return obj;
			}
		}

		T* actor = GetWorld()->SpawnActor<T>(Loc, Rot);
		if (actor)
		{
			_objectPools[name].Add(actor);
		}
		return actor;
	}

	template<typename T>
	typename TEnableIf<TIsDerivedFrom<T,AActor>::Value,void>::type TakeActor(T* actor)
	{
		if (actor == nullptr)
			return;
		
		AActor* obj = Cast<AActor>(actor);

		FName name = T::StaticClass()->GetFName();
		if (_objectPools.Contains(name))
			_objectPools.Add(name, TArray<AActor*>());

		DisableActor(obj);
		TArray<AActor*> children;

		obj->GetAttachedActors(children);
		for (auto child : children)
		{
			DisableActor(child);
		}
		_objectPools[name].Add(obj);
	}


	void			DisableActor(AActor* actor);

	void			EnableActor(AActor* actor);

private:

	TMap<FName, TArray<AActor*>> _objectPools;
	
};
