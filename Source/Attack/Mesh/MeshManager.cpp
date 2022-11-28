// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshManager.h"

namespace RESOURCE
{
	const TMap<FName, const TCHAR*> SKELETAL_MESH_PATHS =
	{
		TPair<FName,const TCHAR*>(FName("SwordMesh"),TEXT("/Game/Resources/InfinityBladeWeapons/Weapons/Dual_Blade/Dual_Blade_Arrow/SK_Dual_Blade_Arrow.SK_Dual_Blade_Arrow")),
	};
}

UMeshManager::UMeshManager()
{
	for (auto& namePath : RESOURCE::SKELETAL_MESH_PATHS)
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh(namePath.Value);
		if (mesh.Succeeded())
		{
			_skeletalMeshes.Add(namePath.Key, mesh.Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Resource Not Found"), *namePath.Key.ToString());
		}
	}

}