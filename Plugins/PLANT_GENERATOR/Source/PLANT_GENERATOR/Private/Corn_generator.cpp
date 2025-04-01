// Fill out your copyright notice in the Description page of Project Settings.


#include "Corn_generator.h"
#include "Util.h"
#include "Engine/StaticMeshActor.h"

Corn_generator::Corn_generator()
{
	m_leaf_HueVariation = 0.15f;
	m_leaf_SaturationVariation = 0.2f;
	m_leaf_ValueVariation = 0.2f;
	m_leaf_ScaleRange = FVector2D(1.f, 2.f);
	m_leaf_ShearFactorRange_x_y = FVector2D(0.5f, 1.5f);
	m_leaf_ShearFactorRange_z = FVector2D(0.25f, 3.f);

	m_stem_HueVariation = 0.15f;
	m_stem_SaturationVariation = 0.1f;
	m_stem_ValueVariation = 0.1f;
	m_stem_ScaleRange_x_y = FVector2D(1.0f, 2.0f);
	m_stem_ScaleRange_z = FVector2D(0.1f, 0.9f);
}

Corn_generator::~Corn_generator()
{
}

AActor* Corn_generator::SpawnActor(TArray<UStaticMeshComponent*> Components, TArray<FVector3d> RelativePositions,
                                FVector3d ActorLocation)
{
	// Ensure both arrays have the same number of elements
	if (Components.Num() != RelativePositions.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Components and RelativePositions arrays must have the same number of elements."));
		return nullptr;
	}

	// Create an actor (AStaticMeshActor) to hold the transformed components
	AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(ActorLocation, FRotator::ZeroRotator);
	if (NewActor)
	{
		// Iterate through the provided components and their relative positions
		for (int i = 0; i < Components.Num(); i++)
		{
			UStaticMeshComponent* Component = Components[i];
			FVector3d RelativePosition = RelativePositions[i];

			if (Component)
			{
				// Ensure component is not already attached to another actor
				if (Component->GetOwner() != nullptr)
				{
					// Detach from the previous owner
					Component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
					Component->UnregisterComponent();
				}

				// Set NewActor as the component's new outer (effectively changing ownership)
				Component->Rename(nullptr, NewActor);

				// Attach all other components to the root component (or stem)
				Component->AttachToComponent(NewActor->GetRootComponent(),
				                             FAttachmentTransformRules::KeepRelativeTransform);

				// Re-register the component so it updates properly
				Component->RegisterComponent();

				// Set the relative location based on the passed parameter
				Component->SetRelativeLocation(FVector(RelativePosition));
			}
		}

		// Finally, set the actor's location (it will position all components relative to the actor)
		NewActor->SetActorLocation(ActorLocation);
	}

	return NewActor;
	
}

UStaticMeshComponent* Corn_generator::CreateLeafVariation(float plantage)
{
	UStaticMesh* RandomLeaf = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Leaves/");
	UStaticMesh* NewLeaf = DuplicateObject(RandomLeaf, nullptr); // Ensure full duplication
	UStaticMeshComponent* NewLeafComponent = NewObject<UStaticMeshComponent>(NewLeaf);
	NewLeafComponent->SetStaticMesh(NewLeaf);

	FVector NewScale = FVector(
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
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

	// Apply material variations
	UMaterialInterface* OriginalMaterial = RandomLeaf->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, nullptr);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("Hue"),
		                                         (1 - plantage) * m_leaf_HueVariation + FMath::FRandRange(-0.05, 0.05));
		DynamicMaterial->SetScalarParameterValue(FName("Saturation"),
		(1 - plantage) * m_leaf_SaturationVariation + FMath::FRandRange(-0.05, 0.05));
		DynamicMaterial->SetScalarParameterValue(FName("Value"),
		(1 - plantage) * m_leaf_ValueVariation + FMath::FRandRange(-0.05, 0.05));

		// Apply the dynamic material to the mesh
		NewLeafComponent->SetMaterial(0, nullptr); // Remove reference before setting
		NewLeafComponent->SetMaterial(0, DynamicMaterial);
	}

	return NewLeafComponent;
}

UStaticMeshComponent* Corn_generator::CreateStemVariation(float plantage)
{
	UStaticMesh* RandomStem = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Stem/");
	UStaticMesh* NewStem = DuplicateObject(RandomStem, nullptr);
	UStaticMeshComponent* NewStemComponent = NewObject<UStaticMeshComponent>(NewStem);
	NewStemComponent->SetStaticMesh(NewStem);

	FVector NewScale = FVector(
	FMath::Clamp(
		m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
		m_stem_ScaleRange_x_y.X,
		m_stem_ScaleRange_x_y.Y
	),
		FMath::Clamp(
			m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
			m_stem_ScaleRange_x_y.X,
			m_stem_ScaleRange_x_y.Y
		),
		FMath::Clamp(
			m_stem_ScaleRange_z.X + (m_stem_ScaleRange_z.Y - m_stem_ScaleRange_z.X) * plantage + FMath::RandRange(-0.1f, 0.1f),
			m_stem_ScaleRange_z.X,
			m_stem_ScaleRange_z.Y
		)
	);

	NewStemComponent->SetWorldScale3D(NewScale);

	// Apply material variations
	UMaterialInterface* OriginalMaterial = RandomStem->GetMaterial(0);
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, nullptr);

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(FName("Hue"),
		                                         (1 - plantage) * m_stem_HueVariation + FMath::FRandRange(-0.05, 0.05));
		DynamicMaterial->SetScalarParameterValue(FName("Saturation"),
		                                         FMath::FRandRange(-m_stem_SaturationVariation,
		                                                           m_stem_SaturationVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Value"),
		                                         FMath::FRandRange(-m_stem_ValueVariation, m_stem_ValueVariation));

		// Apply the dynamic material to the mesh
		NewStemComponent->SetMaterial(0, nullptr);
		NewStemComponent->SetMaterial(0, DynamicMaterial);
	}

	return NewStemComponent;
}

void Corn_generator::CreateVariation(int amount, float plantage, FString exportPath)
{
	int plants_per_row = 10;

	for (int i = 0; i < amount; i++)
	{
		// Create an array to hold all the leaf components
		TArray<UStaticMeshComponent*> leafComponents;
		TArray<FVector3d> RelativePositions;

		// Create the stem component
		UStaticMeshComponent* NewStemComponent = this->CreateStemVariation(plantage);

		// Get the stem's local bounds (without world transform)
		FVector stemOrigin, stemExtents;
		NewStemComponent->GetLocalBounds(stemOrigin, stemExtents); // This gives the local bounds of the mesh

		// Calculate the height based on the local bounds' Z extent
		float stemHeight = stemExtents.Z * NewStemComponent->GetComponentTransform().GetScale3D().Z;
		// Get the full height based on the mesh's Z extents

		// Set the Z-position range based on the stem height (you can adjust this logic as needed)
		FVector2d zPositionRange(0.0, stemHeight);

		// Create leaves and position them along the stem
		int numLeaves = FMath::FloorToInt(FMath::RandRange(4, 8) * (stemHeight / 50));

		float zStep = (zPositionRange.Y - zPositionRange.X) / (numLeaves / 2 + 1);

		for (int j = 0; j < numLeaves; j++)
		{
			UStaticMeshComponent* leaf = this->CreateLeafVariation(plantage);
			leafComponents.Add(leaf);

			// Set relative position for each leaf
			FVector3d relativePos(0.0f, 0.0f, zPositionRange.X + (FMath::Floor(j / 2) + 1) * zStep);
			RelativePositions.Add(relativePos);

			// Apply random rotation (±180 degrees)
			float rotationAngle = j * (180.0f + FMath::RandRange(-20.0f, 20.0f));
			leaf->SetRelativeRotation(FRotator(0.0f, rotationAngle, 0.0f));
		}

		// Add the stem as the first component
		leafComponents.Add(NewStemComponent);
		RelativePositions.Add(FVector3d(0.0f, 0.0f, 0.0f)); // Stem at the base of the plant

		// Spawn the actor with all components (leaves and stem)
		AActor* SpawnedActor = SpawnActor(leafComponents, RelativePositions,
				   FVector(FMath::FloorToInt((float)i / plants_per_row) * 100, (i % plants_per_row) * 100, 0.0f));
		
	}
}