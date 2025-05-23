#include "Carrot_generator.h"
#include "Util.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceConstant.h"

Carrot_generator::Carrot_generator()
{
	m_carrot_MinHueVariation = 0.02f;
	m_carrot_MaxHueVariation = 0.05f;
	m_carrot_SaturationVariation = 0.05f;
	m_carrot_ValueVariation = 0.05f;
	m_carrot_ScaleRangeXZ = FVector2d(0.5f, 1.5f);
	m_carrot_ScaleRangeY = FVector2d(0.5f, 1.5f);
	m_carrot_ShearRange = FVector2d(0.75f, 1.25f);
}

Carrot_generator::~Carrot_generator()
{
}

/*
 * VALID PARAMETERS:
 *    cracked: 0 for clean carrot, 1 for cracked and rotten carrot
 */

AActor* Carrot_generator::CreateVariation(TMap<FString, float> parameters, FTransform startTransform)
{

	bool cracked = (bool)parameters["cracked"];
	
	AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(startTransform.GetLocation(), startTransform.GetRotation().Rotator());

	UStaticMesh* RandomCarrot;
	
	if (cracked == true){
		RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot/cracked_carrots/");
	}
	else
	{
		RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot");
	}
	
	UStaticMeshComponent* NewCarrotComponent = Cast<UStaticMeshComponent>(NewActor->GetRootComponent());
	NewCarrotComponent->SetMobility(EComponentMobility::Movable);
	NewCarrotComponent->SetStaticMesh(RandomCarrot);

	FString PackagePath = TEXT("/game/generated_materials");
	FString AssetName = TEXT("generated_material");

	// Create the material instance asset
	UMaterialInstanceConstant* DynMaterial = Util::CreateMaterialInstance(RandomCarrot->GetMaterial(0), PackagePath, AssetName);

	// Apply random hue, saturation, value variation to the dynamic material
	float Hue = FMath::FRandRange(-m_carrot_MinHueVariation, m_carrot_MaxHueVariation);
	float Value = FMath::FRandRange(-m_carrot_ValueVariation, m_carrot_ValueVariation);
	float Saturation = FMath::FRandRange(-m_carrot_SaturationVariation, m_carrot_SaturationVariation);

	// Assuming the material has parameters named "Hue", "Value", "Saturation", "IsCracked"
	DynMaterial->SetScalarParameterValueEditorOnly(FName("Hue"), Hue);
	DynMaterial->SetScalarParameterValueEditorOnly(FName("Value"), Value);
	DynMaterial->SetScalarParameterValueEditorOnly(FName("Saturation"), Saturation);
	DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtTextureChoice"), FMath::RandBool());
	DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtStrength"), FMath::FRandRange(0.f, 0.5f));

	float BlackSpotShift = FMath::FRandRange(0.f, 100.f);

	if (cracked)
	{
		DynMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotShift"), BlackSpotShift);
		DynMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotEnabled"), 1.0f);
	}

	// Save the material instance asset
	Util::SavePackage(DynMaterial);

	// Apply the saved material to the component
	NewCarrotComponent->SetMaterial(0, DynMaterial);
	
	if (cracked)
	{
		
		UMaterialInstanceConstant* CrackedMaterial = Util::CopyMaterialInstanceConstant(DynMaterial, AssetName, PackagePath);
		CrackedMaterial->SetScalarParameterValueEditorOnly(FName("TextureChoice"), 1.0f);
		Util::SavePackage(CrackedMaterial);
		
		NewCarrotComponent->SetMaterial(1, CrackedMaterial);

		UMaterialInstanceConstant* NoiseMaterial = Util::CreateMaterialInstance(Util::LoadMaterialByName(TEXT("/PLANT_GENERATOR/Carrot/noise_mat")), PackagePath, AssetName);
		NoiseMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotShift"), BlackSpotShift);
		Util::SavePackage(NoiseMaterial);
		NewCarrotComponent->SetMaterial(2, NoiseMaterial);

		// UMaterialInterface* NoiseMaterial = );
		//
		// BaseAssetName = FString::Printf(TEXT("CarrotNoise_%d"), i);
		// AssetName = BaseAssetName;
		// PackagePath = "/Game/GeneratedMaterials";
		// PackageName = PackagePath + "/" + AssetName;
		// Suffix = 1;
		//
		// while (FPackageName::DoesPackageExist(PackageName))
		// {
		// 	AssetName = FString::Printf(TEXT("%s_%d"), *BaseAssetName, Suffix++);
		// 	PackageName = PackagePath + "/" + AssetName;
		// }
		//
		// Package = CreatePackage(*PackageName);
		//
		// DynMaterial = CreateMaterialInstance(NoiseMaterial, PackageName, AssetName);
		//
		// DynMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotShift"), FMath::FRandRange(0.f, 1.f));
		//
		// SavePackage(DynMaterial, PackageName, Package);
		//
		// NewCarrotComponent->SetMaterial(2, DynMaterial);

	}
	
	FVector NewScale = FVector(
	FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y),
	FMath::RandRange(m_carrot_ScaleRangeY.X, m_carrot_ScaleRangeY.Y),
	FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y)
	);
	
	NewCarrotComponent->SetWorldScale3D(NewScale);

	// // Create random rotation around Z axis (0-360 degrees)
	// FRotator RandomRotation(0, FMath::RandRange(0.f, 360.f), 0);

	// NewCarrotComponent->SetWorldRotation(RandomRotation);

	FMatrix TransformMatrix = NewCarrotComponent->GetComponentTransform().ToMatrixWithScale();
	FVector Shear = FVector(
		FMath::RandRange(m_carrot_ShearRange.X, m_carrot_ShearRange.Y),
		FMath::RandRange(m_carrot_ShearRange.X, m_carrot_ShearRange.Y),
		FMath::RandRange(m_carrot_ShearRange.X, m_carrot_ShearRange.Y)
	);

	FMatrix ShearMatrix = FMatrix::Identity;
	ShearMatrix.M[0][1] = Shear[0];
	ShearMatrix.M[0][2] = Shear[0];
	ShearMatrix.M[1][0] = Shear[1];
	ShearMatrix.M[1][2] = Shear[1];
	ShearMatrix.M[2][0] = Shear[2];
	ShearMatrix.M[2][1] = Shear[2];
	FMatrix NewMatrix = ShearMatrix * TransformMatrix;

	FTransform Transform = FTransform(NewMatrix);

	NewCarrotComponent->SetWorldTransform(Transform);

	return NewActor;
}
