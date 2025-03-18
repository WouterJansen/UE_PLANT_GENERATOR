// Fill out your copyright notice in the Description page of Project Settings.


#include "Corn_generator.h"

#include <string>

#include "Util.h"
#include "Engine/StaticMeshActor.h"

Corn_generator::Corn_generator()
{
}

Corn_generator::~Corn_generator()
{
}

UStaticMesh* Corn_generator::CreateVariations(FVector2D ScaleRange,
						  float HueVariation, 
						  float SaturationVariation, 
						  float ValueVariation, 
						  FVector2D ShearFactorRange)
{
	// Get random leaf name
	
	UStaticMesh* RandomLeaf = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Leaves/");

	if (RandomLeaf)
	{
		UE_LOG(LogTemp, Log, TEXT("Random Mesh Loaded: %s"), *RandomLeaf->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Random Mesh Not Loaded"));
	}

	UStaticMesh* NewLeaf = DuplicateObject(RandomLeaf, RandomLeaf->GetOuter());
	UStaticMeshComponent* NewLeafComponent = NewObject<UStaticMeshComponent>(RandomLeaf->GetOuter());
	NewLeafComponent->SetStaticMesh(NewLeaf);

	FVector NewScale = FVector(
			FMath::RandRange(ScaleRange.X, ScaleRange.Y),
			FMath::RandRange(ScaleRange.X, ScaleRange.Y),
			FMath::RandRange(ScaleRange.X, ScaleRange.Y)
		);
	
    NewLeafComponent->SetWorldScale3D(NewScale);

	FMatrix TransformMatrix = NewLeafComponent->GetComponentTransform().ToMatrixWithScale();
	FVector Shear = FVector(
			FMath::RandRange(ShearFactorRange.X, ShearFactorRange.Y),
			FMath::RandRange(ShearFactorRange.X, ShearFactorRange.Y),
			FMath::RandRange(ShearFactorRange.X, ShearFactorRange.Y)
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

		DynamicMaterial->SetScalarParameterValue(FName("Hue"), FMath::FRandRange(0.f, HueVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Saturation"), FMath::FRandRange(-SaturationVariation, SaturationVariation));
		DynamicMaterial->SetScalarParameterValue(FName("Value"), FMath::FRandRange(-ValueVariation, ValueVariation));
		
		// Apply the dynamic material to the mesh
		NewLeafComponent->SetMaterial(0, DynamicMaterial);
	}
	// Create an actor (AStaticMeshActor) to hold the transformed component
	AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(NewLeafComponent->GetComponentLocation(), NewLeafComponent->GetComponentRotation());
	if (NewActor)
	{
		// Set the transformed component as the actor's root component
		NewActor->GetStaticMeshComponent()->SetStaticMesh(NewLeaf);
		NewActor->GetStaticMeshComponent()->SetWorldScale3D(NewScale);
		NewActor->GetStaticMeshComponent()->SetWorldTransform(Transform);
		NewActor->GetStaticMeshComponent()->SetMaterial(0, DynamicMaterial);
	}
	
	return NewLeaf;
}
