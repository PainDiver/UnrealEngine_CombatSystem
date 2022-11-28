// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Write resource path here

namespace RESOURCE
{
	const TMap<FName, const TCHAR*> PARTICLE_RESOURCE_PATHS = {
		TPair<FName,const TCHAR*>("MuzzleFlash",TEXT("/Game/MyContent/FX/P_ChickenMaster_MuzzleFlash_01.P_ChickenMaster_MuzzleFlash_01")),
		TPair<FName,const TCHAR*>("DeathBackPack",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Gruntling/Notifies/P_Death_backpack_expl_02.P_Death_backpack_expl_02")),
		TPair<FName,const TCHAR*>("DragDirt",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Gruntling/Master/P_MasterGrunt_Drag_Dirt.P_MasterGrunt_Drag_Dirt")),
		TPair<FName,const TCHAR*>("RockBurst",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Skill_RockBurst/P_RBurst_Fire_Burst_Area_01.P_RBurst_Fire_Burst_Area_01")),
		TPair<FName,const TCHAR*>("PowerUp",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Mobile/Fire/combat/P_FireBall_Powerup.P_FireBall_Powerup")),
		TPair<FName,const TCHAR*>("DashTrail",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Mobile/Fire/combat/P_Swipe_Fire_AttackTrail.P_Swipe_Fire_AttackTrail")),
		TPair<FName,const TCHAR*>("Dash",TEXT("/Game/Resources/InfinityBladeEffects/Effects/FX_Breakables/Wall/P_WallBreak_Stone.P_WallBreak_Stone"))

	};
};

UParticleManager::UParticleManager()
{
	for (auto& namePath : RESOURCE::PARTICLE_RESOURCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UParticleSystem> particle(namePath.Value);
		if (particle.Succeeded())
		{
			_particles.Add(namePath.Key, particle.Object);
		}
		else
		{
			// Resource Not Found
		}
	}
}

void UParticleManager::PlayParticleAtLocation(FName name, FVector loc, FVector scale,FRotator rot)
{
	if(_particles.Contains(name))
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _particles[name], loc, rot,scale);
}

UParticleSystemComponent* UParticleManager::PlayParticleAtMesh(FName name, USceneComponent* mesh, FName socket, FVector offset, FVector scale,FRotator offrot)
{
	if (_particles.Contains(name))
	{
		return UGameplayStatics::SpawnEmitterAttached(_particles[name], mesh, socket, offset, offrot, scale);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("No particle"));
		return nullptr;
	}
}

