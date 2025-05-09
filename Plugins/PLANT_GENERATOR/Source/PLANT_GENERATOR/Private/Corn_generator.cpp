// Fill out your copyright notice in the Description page of Project Settings.


#include "Corn_generator.h"
#include "Util.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

Corn_generator::Corn_generator()
{
	m_leaf_HueVariation = 0.15f;
	m_leaf_SaturationVariation = 0.2f;
	m_leaf_ValueVariation = 0.2f;
	m_leaf_ScaleRange = FVector2D(1.f, 1.5f);
	m_leaf_ShearFactorRange_x_y = FVector2D(0.5f, 1.5f);
	m_leaf_ShearFactorRange_z = FVector2D(0.25f, 3.f);

	m_stem_HueVariation = 0.15f;
	m_stem_SaturationVariation = 0.1f;
	m_stem_ValueVariation = 0.1f;
	m_stem_ScaleRange_x_y = FVector2D(1.0f, 2.0f);
	m_stem_ScaleRange_z = FVector2D(0.1f, 0.65f);
}

Corn_generator::~Corn_generator()
{
}

UStaticMeshComponent* Corn_generator::CreateLeafVariation(float plantage, AActor* NewActor)
{
	UStaticMesh* RandomLeaf = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Leaves/");
	UInstancedStaticMeshComponent* NewLeafComponent =  Cast<UInstancedStaticMeshComponent>(NewActor->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform(), false));
	NewLeafComponent->SetMobility(EComponentMobility::Movable);
	NewActor->FinishAddComponent(NewLeafComponent, false, FTransform());
	NewActor->AddInstanceComponent(NewLeafComponent);
	NewLeafComponent->SetStaticMesh(RandomLeaf);
	NewLeafComponent->AddInstance(FTransform());
	NewLeafComponent->SetNumCustomDataFloats(3);

	//Square root scaling for leaf scale -> if plants get really old, they dont grow anymore

	FVector NewScale = FVector(
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		)
	);

	NewLeafComponent->SetWorldScale3D(NewScale);

	FMatrix TransformMatrix = NewLeafComponent->GetComponentTransform().ToMatrixWithScale();
	FVector Shear = FVector(
		FMath::RandRange(m_leaf_ShearFactorRange_x_y.X, m_leaf_ShearFactorRange_x_y.Y),
		FMath::RandRange(m_leaf_ShearFactorRange_x_y.X, m_leaf_ShearFactorRange_x_y.Y),
		FMath::Clamp(
			m_leaf_ShearFactorRange_z.X + (m_leaf_ShearFactorRange_z.Y - m_leaf_ShearFactorRange_z.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ShearFactorRange_z.X,
			m_leaf_ShearFactorRange_z.Y
		)
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

	NewLeafComponent->SetWorldTransform(Transform);

	NewLeafComponent->SetCustomDataValue(0, 0, (1 - plantage) * m_leaf_HueVariation + FMath::FRandRange(-0.05, 0.05));
	NewLeafComponent->SetCustomDataValue(0, 1, (1 - plantage) * m_leaf_SaturationVariation + FMath::FRandRange(-0.05, 0.05));
	NewLeafComponent->SetCustomDataValue(0, 2, (1 - plantage) * m_leaf_ValueVariation + FMath::FRandRange(-0.05, 0.05));

	return NewLeafComponent;
}

UStaticMeshComponent* Corn_generator::CreateStemVariation(float plantage, AActor* NewActor)
{
	UStaticMesh* RandomStem = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Stem/");
	UInstancedStaticMeshComponent* NewStemComponent =  Cast<UInstancedStaticMeshComponent>(NewActor->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform(), false));
	NewStemComponent->SetMobility(EComponentMobility::Movable);
	NewActor->FinishAddComponent(NewStemComponent, false, FTransform());
	NewActor->AddInstanceComponent(NewStemComponent);
	NewStemComponent->SetStaticMesh(RandomStem);
	NewStemComponent->AddInstance(FTransform());
	NewStemComponent->SetNumCustomDataFloats(3);

	//Square root scaling for stem scale -> if plants get really old, they dont grow anymore

	FVector NewScale = FVector(
	FMath::Clamp(
		m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
		m_stem_ScaleRange_x_y.X,
		m_stem_ScaleRange_x_y.Y
	),
		FMath::Clamp(
			m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
			m_stem_ScaleRange_x_y.X,
			m_stem_ScaleRange_x_y.Y
		),
		FMath::Clamp(
		  m_stem_ScaleRange_z.X + (m_stem_ScaleRange_z.Y - m_stem_ScaleRange_z.X) * FMath::Sqrt(plantage) + FMath::RandRange(-0.1f, 0.1f),
		m_stem_ScaleRange_z.X,
		m_stem_ScaleRange_z.Y
		)
	);

	NewStemComponent->SetWorldScale3D(NewScale);

	NewStemComponent->SetCustomDataValue(0, 0, (1 - plantage) * m_stem_HueVariation + FMath::FRandRange(-0.05, 0.05));
	NewStemComponent->SetCustomDataValue(0, 1, (1 - plantage) * m_stem_SaturationVariation + FMath::FRandRange(-0.05, 0.05));
	NewStemComponent->SetCustomDataValue(0, 2, (1 - plantage) * m_stem_ValueVariation + FMath::FRandRange(-0.05, 0.05));
	
	return NewStemComponent;
}

void Corn_generator::CreateVariation(int amount, float plantage, FString exportPath)
{
	int plants_per_row = 10;
	
	for (int i = 0; i < amount; i++)
	{

		AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(FVector(FMath::FloorToInt((float)i / plants_per_row) * 100, (i % plants_per_row) * 100, 0.0f), FRotator::ZeroRotator);
		
		// Create the stem component
		UStaticMeshComponent* NewStemComponent = this->CreateStemVariation(plantage, NewActor);

		// Get the stem's local bounds (without world transform)
		FVector stemOrigin, stemExtents;
		NewStemComponent->GetLocalBounds(stemOrigin, stemExtents); // This gives the local bounds of the mesh

		// Calculate the height based on the local bounds' Z extent
		float stemHeight = stemExtents.Z * NewStemComponent->GetComponentTransform().GetScale3D().Z;

		FVector2d zPositionRange(0.0, stemHeight);

		// Create leaves and position them along the stem
		int numLeaves = FMath::FloorToInt(FMath::RandRange(4, 8) * (stemHeight / 65));

		float zStep = (zPositionRange.Y - zPositionRange.X) / (numLeaves / 2 + 1);

		for (int j = 0; j < numLeaves; j++)
		{
			UStaticMeshComponent* leaf = this->CreateLeafVariation(plantage, NewActor);

			// Set relative position for each leaf
			FVector3d relativePos(0.0f, 0.0f, zPositionRange.X + (FMath::Floor(j / 2) + 1) * zStep);
			leaf->SetRelativeLocation(relativePos);

			// Apply random rotation (±180 degrees)
			float rotationAngle = j * (180.0f + FMath::RandRange(-10.0f, 10.0f));
			leaf->SetRelativeRotation(FRotator(0.0f, rotationAngle, 0.0f));
		}
	}
}