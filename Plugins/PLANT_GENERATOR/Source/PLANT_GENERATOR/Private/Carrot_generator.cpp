#include "Carrot_generator.h"

#include "AssetToolsModule.h"
#include "Util.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"

Carrot_generator::Carrot_generator()
{
	m_carrot_MinHueVariation = 0.02f;
	m_carrot_MaxHueVariation = 0.05f;
	m_carrot_SaturationVariation = 0.05f;
	m_carrot_ValueVariation = 0.05f;
	m_carrot_ScaleRangeXZ = FVector2d(0.5f, 1.5f);
	m_carrot_ScaleRangeY = FVector2d(0.5f, 1.5f);
	m_carrot_ShearRange = FVector2d(0.5f, 1.5f);
}

Carrot_generator::~Carrot_generator()
{
}

void Carrot_generator::CreateVariation(int amount, bool cracked)
{

	int carrots_per_row = 25;
	
	for (int i = 0; i < amount; i++)
	{
		AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(FVector(FMath::FloorToInt((float)i / carrots_per_row) * 100, (i % carrots_per_row) * 100, 25), FRotator::ZeroRotator);

		UStaticMesh* RandomCarrot;
		
		if (cracked == true){
			RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot/cracked_carrots");
		}
		else
		{
			RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot");
		}
		
		UStaticMeshComponent* NewCarrotComponent = Cast<UStaticMeshComponent>(NewActor->GetRootComponent());
		NewCarrotComponent->SetMobility(EComponentMobility::Movable);
		NewCarrotComponent->SetStaticMesh(RandomCarrot);

		FString BaseAssetName = FString::Printf(TEXT("CarrotMaterial_%d"), i);
		FString AssetName = BaseAssetName;
		FString PackagePath = "/Game/GeneratedMaterials";
		FString PackageName = PackagePath + "/" + AssetName;
		int32 Suffix = 1;

		while (FPackageName::DoesPackageExist(PackageName))
		{
			AssetName = FString::Printf(TEXT("%s_%d"), *BaseAssetName, Suffix++);
			PackageName = PackagePath + "/" + AssetName;
		}

		UPackage* Package = CreatePackage(*PackageName);

		// Create the material instance asset
		UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
		Factory->InitialParent = RandomCarrot->GetMaterial(0);
		FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
		UMaterialInstanceConstant* DynMaterial = Cast<UMaterialInstanceConstant>(
			AssetToolsModule.Get().CreateAsset(AssetName, FPackageName::GetLongPackagePath(PackageName), UMaterialInstanceConstant::StaticClass(), Factory));

		// Apply random hue, saturation, value variation to the dynamic material
		float Hue = FMath::FRandRange(-m_carrot_MinHueVariation, m_carrot_MaxHueVariation);
		float Value = FMath::FRandRange(-m_carrot_ValueVariation, m_carrot_ValueVariation);
		float Saturation = FMath::FRandRange(-m_carrot_SaturationVariation, m_carrot_SaturationVariation);

		// Assuming the material has parameters named "Hue", "Value", "Saturation", "IsCracked"
		DynMaterial->SetScalarParameterValueEditorOnly(FName("Hue"), Hue);
		DynMaterial->SetScalarParameterValueEditorOnly(FName("Value"), Value);
		DynMaterial->SetScalarParameterValueEditorOnly(FName("Saturation"), Saturation);
		DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtTextureChoice"), FMath::RandBool());
		DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtMaskChoice"), FMath::RandBool());
		DynMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotEnabled"), cracked ? 1 : 0);
		DynMaterial->SetScalarParameterValueEditorOnly(FName("BlackSpotShift"), FMath::FRandRange(0.f, 1.f));
		DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtTextureShift"), FMath::FRandRange(0.f, 1.f));
		DynMaterial->SetScalarParameterValueEditorOnly(FName("DirtStrength"), FMath::FRandRange(0.f, 1.f));

		// Save the material instance asset
		DynMaterial->PostEditChange();
		DynMaterial->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(DynMaterial);
		UPackage::SavePackage(Package, DynMaterial, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()));

		// Apply the saved material to the component
		NewCarrotComponent->SetMaterial(0, DynMaterial);
		
		FVector NewScale = FVector(
		FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y),
		FMath::RandRange(m_carrot_ScaleRangeY.X, m_carrot_ScaleRangeY.Y),
		FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y)
		);
		
		NewCarrotComponent->SetWorldScale3D(NewScale);

		// Create random rotation around Z axis (0-360 degrees)
		FRotator RandomRotation(0, FMath::RandRange(0.f, 360.f), 0);

		NewCarrotComponent->SetWorldRotation(RandomRotation);

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
		
	}

}
