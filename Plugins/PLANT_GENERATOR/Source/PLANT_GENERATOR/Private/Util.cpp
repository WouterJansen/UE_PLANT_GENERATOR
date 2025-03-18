// Fill out your copyright notice in the Description page of Project Settings.


#include "Util.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"


Util::Util()
{
}

Util::~Util()
{
}

UStaticMesh* Util::GetRandomMeshFromFolder(const FString& FolderPath)
{
	// Get the Asset Registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// Store found assets
	TArray<FAssetData> AssetDataList;

	// Get all assets in the specified folder
	FARFilter Filter;
	Filter.PackagePaths.Add(*FolderPath);  // Example: "/Game/Meshes/Corn"
	Filter.bRecursivePaths = false;  // Set to true if you want to include subfolders
	Filter.ClassPaths.Add(UStaticMesh::StaticClass()->GetClassPathName());

	// Query the Asset Registry
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() > 0)
	{
		// Pick a random asset from the list
		int32 RandomIndex = FMath::RandRange(0, AssetDataList.Num() - 1);
		FAssetData RandomAsset = AssetDataList[RandomIndex];

		// Load and return the UStaticMesh
		return Cast<UStaticMesh>(RandomAsset.GetAsset());
	}

	// If no meshes were found, log a warning
	UE_LOG(LogTemp, Warning, TEXT("No static meshes found in folder: %s"), *FolderPath);
	return nullptr;
}