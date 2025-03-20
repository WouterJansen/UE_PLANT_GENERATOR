// Fill out your copyright notice in the Description page of Project Settings.


#include "Corn_generator.h"

#include <string>
#include "Util.h"
#include "Engine/StaticMeshActor.h"

Corn_generator::Corn_generator()
{
	m_leaf_HueVariation = 0.15f;
	m_leaf_SaturationVariation = 0.2f;
	m_leaf_ValueVariation = 0.2f;
	m_leaf_ScaleRange = FVector2D(0.25f, 4.0f);
	m_leaf_ShearFactorRange = FVector2D(-0.5f, 1.0f);

	m_stem_HueVariation = 0.15f;
	m_stem_SaturationVariation = 0.2f;
	m_stem_ValueVariation = 0.2f;
	m_stem_ScaleRange_x_y = FVector2D(1.0f, 2.0f);
	m_stem_ScaleRange_z = FVector2D(0.5f, 1.5f);
}

Corn_generator::~Corn_generator()
{
}

void Corn_generator::SpawnActor(TArray<UStaticMeshComponent*> Components, TArray<FVector3d> RelativePositions, FVector3d ActorLocation)
{
    // Ensure both arrays have the same number of elements
    if (Components.Num() != RelativePositions.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Components and RelativePositions arrays must have the same number of elements."));
        return;
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
                Component->AttachToComponent(NewActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

                // Re-register the component so it updates properly
                Component->RegisterComponent();

                // Set the relative location based on the passed parameter
                Component->SetRelativeLocation(FVector(RelativePosition));
            }
        }

        // Finally, set the actor's location (it will position all components relative to the actor)
        NewActor->SetActorLocation(ActorLocation);
    }
}

UStaticMeshComponent* Corn_generator::CreateLeafVariation()
{
	
	UStaticMesh* RandomLeaf = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Leaves/");

	UStaticMesh* NewLeaf = DuplicateObject(RandomLeaf, RandomLeaf->GetOuter());
	UStaticMeshComponent* NewLeafComponent = NewObject<UStaticMeshComponent>(RandomLeaf->GetOuter());
	NewLeafComponent->SetStaticMesh(NewLeaf);

	FVector NewScale = FVector(
			FMath::RandRange(m_leaf_ScaleRange.X, m_leaf_ScaleRange.Y),
			FMath::RandRange(m_leaf_ScaleRange.X, m_leaf_ScaleRange.Y),
			FMath::RandRange(m_leaf_ScaleRange.X, m_leaf_ScaleRange.Y)
		);
	
    NewLeafComponent->SetWorldScale3D(NewScale);

	FMatrix TransformMatrix = NewLeafComponent->GetComponentTransform().ToMatrixWithScale();
	FVector Shear = FVector(
			FMath::RandRange(m_leaf_ShearFactorRange.X, m_leaf_ShearFactorRange.Y),
			FMath::RandRange(m_leaf_ShearFactorRange.X, m_leaf_ShearFactorRange.Y),
			FMath::RandRange(m_leaf_ShearFactorRange.X, m_leaf_ShearFactorRange.Y)
		);

	FMatrix ShearMatrix = FMatrix::Identity;
	ShearMatrix.M[1][0] = Shear[0];
	ShearMatrix.M[2][0] = Shear[1];

	FMatrix NewMatrix = ShearMatrix * TransformMatrix;

	FTransform Transform = FTransform(NewMatrix);
	
	NewLeafComponent->SetWorldTransform(Transform);

	// Apply material variations
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(NewLeafComponent->GetMaterial(0), RandomLeaf->GetOuter());

	if (DynamicMaterial)
	{

		DynamicMaterial->SetScalarParameterValue(FName("Hue"), FMath::FRandRange(0.f, m_leaf_HueVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Saturation"), FMath::FRandRange(-m_leaf_SaturationVariation, m_leaf_SaturationVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Value"), FMath::FRandRange(-m_leaf_ValueVariation, m_leaf_ValueVariation));
		
		// Apply the dynamic material to the mesh
		NewLeafComponent->SetMaterial(0, DynamicMaterial);
	}

	return NewLeafComponent;
	
}

UStaticMeshComponent* Corn_generator::CreateStemVariation()
{
	
	UStaticMesh* RandomStem = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Stem/");

	UStaticMesh* NewStem = DuplicateObject(RandomStem, RandomStem->GetOuter());
	UStaticMeshComponent* NewStemComponent = NewObject<UStaticMeshComponent>(RandomStem->GetOuter());
	NewStemComponent->SetStaticMesh(NewStem);

	FVector NewScale = FVector(
			FMath::RandRange(m_stem_ScaleRange_x_y.X, m_stem_ScaleRange_x_y.Y),
			FMath::RandRange(m_stem_ScaleRange_x_y.X, m_stem_ScaleRange_x_y.Y),
			FMath::RandRange(m_stem_ScaleRange_z.X, m_stem_ScaleRange_z.Y)
		);
	
	NewStemComponent->SetWorldScale3D(NewScale);

	// Apply material variations
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(NewStemComponent->GetMaterial(0), RandomStem->GetOuter());

	if (DynamicMaterial)
	{

		DynamicMaterial->SetScalarParameterValue(FName("Hue"), FMath::FRandRange(0.f, m_stem_HueVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Saturation"), FMath::FRandRange(-m_stem_SaturationVariation, m_stem_SaturationVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Value"), FMath::FRandRange(-m_stem_ValueVariation, m_stem_ValueVariation));
		
		// Apply the dynamic material to the mesh
		NewStemComponent->SetMaterial(0, DynamicMaterial);
	}

	return NewStemComponent;
	
}

void Corn_generator::CreateVariation()
{

	TArray<UStaticMeshComponent*> leafComponents;
	TArray<FVector3d> RelativePositions;

	for (int i = 0; i < 10; i++)
	{
		leafComponents.Add(CreateLeafVariation());
		RelativePositions.Add(FVector3d(0, i*5, 0));
	}

	UStaticMeshComponent* NewStemComponent = this->CreateStemVariation();

	leafComponents.Add(NewStemComponent);
	RelativePositions.Add(FVector3d(0, 0, 0));

	SpawnActor(leafComponents, RelativePositions, FVector(0, 0, 0));
	
}
