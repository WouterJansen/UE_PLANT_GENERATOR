#include "ACarrotActor.h"
#include "Util.h"

ACarrotActor::ACarrotActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Carrot = this->GetStaticMeshComponent();
	Carrot->SetMobility(EComponentMobility::Movable);

	m_carrot_MinHueVariation = 0.02f;
	m_carrot_MaxHueVariation = 0.05f;
	m_carrot_SaturationVariation = 0.05f;
	m_carrot_ValueVariation = 0.05f;
	m_carrot_ScaleRangeXZ = FVector2d(0.5f, 1.5f);
	m_carrot_ScaleRangeY = FVector2d(0.5f, 1.5f);
	m_carrot_ShearRange = FVector2d(0.75f, 1.25f);

}

void ACarrotActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACarrotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACarrotActor::GenerateCarrot(TMap<FString, float> parameters)
{
	bool cracked = (bool)parameters.FindRef("Cracked", false);
	
	UStaticMesh* RandomCarrot;
	
	if (cracked == true){
		RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot/cracked_carrots/");
	}
	else
	{
		RandomCarrot = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Carrot");
	}
	
	Carrot->SetStaticMesh(RandomCarrot);

	FString PackagePath = TEXT("/game/generated_materials");
	FString AssetName = TEXT("generated_material");

	// Create the material instance asset
	UMaterialInterface* BaseMaterial = RandomCarrot->GetMaterial(0);
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

	// Apply random hue, saturation, value variation to the dynamic material
	float Hue = FMath::FRandRange(-m_carrot_MinHueVariation, m_carrot_MaxHueVariation);
	float Value = FMath::FRandRange(-m_carrot_ValueVariation, m_carrot_ValueVariation);
	float Saturation = FMath::FRandRange(-m_carrot_SaturationVariation, m_carrot_SaturationVariation);

	// Assuming the material has parameters named "Hue", "Value", "Saturation", "IsCracked"
	DynMaterial->SetScalarParameterValue(FName("Hue"), Hue);
	DynMaterial->SetScalarParameterValue(FName("Value"), Value);
	DynMaterial->SetScalarParameterValue(FName("Saturation"), Saturation);
	DynMaterial->SetScalarParameterValue(FName("DirtTextureChoice"), FMath::RandBool());
	DynMaterial->SetScalarParameterValue(FName("DirtStrength"), FMath::FRandRange(0.f, 0.5f));

	float BlackSpotShift = FMath::FRandRange(0.f, 100.f);

	if (cracked)
	{
		DynMaterial->SetScalarParameterValue(FName("BlackSpotShift"), BlackSpotShift);
		DynMaterial->SetScalarParameterValue(FName("BlackSpotEnabled"), 1.0f);
	}

	// Apply the saved material to the component
	Carrot->SetMaterial(0, DynMaterial);
	
	if (cracked)
	{
		UMaterialInstanceDynamic* DynMaterialCrack = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		DynMaterialCrack->CopyInterpParameters(DynMaterial);

		DynMaterialCrack->SetScalarParameterValue(FName("TextureChoice"), 1.0f);
		
		Carrot->SetMaterial(1, DynMaterialCrack);
	}
	
	FVector NewScale = FVector(
	FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y),
	FMath::RandRange(m_carrot_ScaleRangeY.X, m_carrot_ScaleRangeY.Y),
	FMath::RandRange(m_carrot_ScaleRangeXZ.X, m_carrot_ScaleRangeXZ.Y)
	);
	
	Carrot->SetWorldScale3D(NewScale);

	FMatrix TransformMatrix = Carrot->GetComponentTransform().ToMatrixWithScale();
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

	Carrot->SetWorldTransform(Transform);
}

