#include "AppleActor.h"
#include "Util.h"

AAppleActor::AAppleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Apple = this->GetStaticMeshComponent();
	Apple->SetMobility(EComponentMobility::Movable);

	m_MinHueVariation = 0.02f;
	m_MaxHueVariation = 0.02f;
	m_SaturationVariation = 0.02f;
	m_ValueVariation = 0.02f;

}

void AAppleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAppleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAppleActor::GenerateApple(TMap<FString, float> parameters)
{
	UStaticMesh* RandomAppleMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Apple/"));
	Apple->SetStaticMesh(RandomAppleMesh);

	Apple->SetWorldScale3D(FVector(10, 10, 10));

	// Create the material instance asset
	UMaterialInterface* BaseMaterial = Apple->GetMaterial(0);
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

	float Hue = FMath::FRandRange(-m_MinHueVariation, m_MaxHueVariation);
	float Value = FMath::FRandRange(-m_ValueVariation, m_ValueVariation);
	float Saturation = FMath::FRandRange(-m_SaturationVariation, m_SaturationVariation);
	
	DynMaterial->SetScalarParameterValue("Hue", Hue);
	DynMaterial->SetScalarParameterValue("Value", Value);
	DynMaterial->SetScalarParameterValue("Sat", Saturation);
	DynMaterial->SetScalarParameterValue("Brightness", Saturation + 1);
	DynMaterial->SetScalarParameterValue("Contrast", Saturation + 1);

	Apple->SetMaterial(0, DynMaterial);
	
}

