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

void Corn_generator::SpawnActor(UStaticMeshComponent* component, FVector3d location)
{
	// Create an actor (AStaticMeshActor) to hold the transformed component
	AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(component->GetComponentLocation(), component->GetComponentRotation());
	if (NewActor && component)
	{
		// Ensure NewLeafComponent is not already attached to another actor
		if (component->GetOwner() != nullptr)
		{
			// Detach from previous owner
			component->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			component->UnregisterComponent();
		}

		// Set NewActor as the component's new outer (effectively changing ownership)
		component->Rename(nullptr, NewActor);
    
		// Set as the new root component
		NewActor->SetRootComponent(component);
    
		// Re-register the component so it updates properly
		component->RegisterComponent();

		NewActor->SetActorLocation(location);	
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

	for (int i = 0; i < 10; i++)
	{
		leafComponents.Add(CreateLeafVariation());
	}

	UStaticMeshComponent* NewStemComponent = this->CreateStemVariation();

	for (int i = 0; i < leafComponents.Num(); i++)
	{
		SpawnActor(leafComponents[i], FVector3d(0, i*2, 0));
	}

	SpawnActor(NewStemComponent, FVector3d(0, 0, 0));
	
}
