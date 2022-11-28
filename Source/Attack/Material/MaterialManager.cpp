// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialManager.h"

namespace RESOURCE
{
	const TMap<FName, const TCHAR*> MATERIAL_INSTANCE_PATHS =
	{
		TPair<FName,const TCHAR*>(FName("RadialBlurMaterial"),TEXT("/Game/MyContent/PostProcess/Radial_Blur_Inst.Radial_Blur_Inst")),
	};

	const TMap<FName, const TCHAR*> MATERIAL_PATHS =
	{
	};
}


UMaterialManager::UMaterialManager()
{
	for (auto& namePath : RESOURCE::MATERIAL_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UMaterial> material(namePath.Value);
		if (material.Succeeded())
		{
			_materials.Add(namePath.Key, material.Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Resource Not Found"), *namePath.Key.ToString());
		}
	}

	for (auto& namePath : RESOURCE::MATERIAL_INSTANCE_PATHS)
	{
		ConstructorHelpers::FObjectFinder<UMaterialInstance> materialInstance(namePath.Value);
		if (materialInstance.Succeeded())
		{
			_materialInstances.Add(namePath.Key, materialInstance.Object);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Resource Not Found"), *namePath.Key.ToString());
		}
	}


}
