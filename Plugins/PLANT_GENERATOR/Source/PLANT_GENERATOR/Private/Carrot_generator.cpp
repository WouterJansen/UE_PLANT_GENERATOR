#include "Carrot_generator.h"
#include "Util.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

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
		UInstancedStaticMeshComponent* NewCarrotComponent =  Cast<UInstancedStaticMeshComponent>(NewActor->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform(), false));
		NewCarrotComponent->SetMobility(EComponentMobility::Movable);
		NewActor->FinishAddComponent(NewCarrotComponent, false, FTransform());
		NewActor->AddInstanceComponent(NewCarrotComponent);
		NewCarrotComponent->SetStaticMesh(RandomCarrot);
		NewCarrotComponent->AddInstance(FTransform());
		NewCarrotComponent->SetNumCustomDataFloats(7);


		NewCarrotComponent->SetCustomDataValue(0, 0, FMath::FRandRange(-m_carrot_MinHueVariation, m_carrot_MaxHueVariation));
		NewCarrotComponent->SetCustomDataValue(0, 1, FMath::FRandRange(-m_carrot_ValueVariation, m_carrot_ValueVariation));
		NewCarrotComponent->SetCustomDataValue(0, 2,FMath::FRandRange(-m_carrot_SaturationVariation, m_carrot_SaturationVariation));

		NewCarrotComponent->SetCustomDataValue(0, 3, FMath::FRandRange(0.0f, 1.0f));
		NewCarrotComponent->SetCustomDataValue(0, 4, FMath::FRandRange(0.0f, 1.0f));
		NewCarrotComponent->SetCustomDataValue(0, 5, FMath::FRandRange(0.0f, 1.0f));
		NewCarrotComponent->SetCustomDataValue(0, 6, FMath::FRandRange(0.0f, 1.0f));

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
