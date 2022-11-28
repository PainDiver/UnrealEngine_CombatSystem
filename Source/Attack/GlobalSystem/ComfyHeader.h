// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define COMFYDEBUG(s, ...) if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT(s), ##__VA_ARGS__));
#define COMFYLOG(s, ...) UE_LOG(LogTemp, Warning, TEXT(s),##__VA_ARGS__);

#define GAMEINSTANCE				Cast<UAttackGameInstance>(GetWorld()->GetGameInstance())

#define PARTICLE_MANAGER			GAMEINSTANCE->GetParticleManager()
#define SOUND_MANAGER				GAMEINSTANCE->GetSoundManager()
#define CAMERA_SHAKE_MANAGER		GAMEINSTANCE->GetCameraShakeManager()
#define MESH_MANAGER				GAMEINSTANCE->GetMeshManager()
#define ANIMATION_MANAGER			GAMEINSTANCE->GetAnimationManager()
#define MATERIAL_MANAGER			GAMEINSTANCE->GetMaterialManager()
#define TIME_MANAGER				GAMEINSTANCE->GetTimeManager()
#define AI_MANAGER					GAMEINSTANCE->GetAIManager()
