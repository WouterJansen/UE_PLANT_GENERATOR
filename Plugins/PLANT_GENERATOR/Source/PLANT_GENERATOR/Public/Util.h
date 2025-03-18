// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PLANT_GENERATOR_API Util
{
public:
	Util();
	~Util();
	static UStaticMesh* GetRandomMeshFromFolder(const FString& FolderPath);
};
