#include "CornActor.h"
#include "Util.h"

ACornActor::ACornActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	SceneRoot->SetMobility(EComponentMobility::Movable); 

	CornStem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RachisSpline"));
	CornStem->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform);
	CornStem->SetMobility(EComponentMobility::Movable);

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

void ACornActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACornActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACornActor::GenerateCorn(TMap<FString, float> parameters)
{
	float plantAge = parameters.FindRef("PlantAge", 0.5);
	float leafDensity = parameters.FindRef("LeafDensity", 1.0f);
	
	UStaticMesh* RandomStem = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Stem/");
	CornStem->SetStaticMesh(RandomStem);

	FVector NewScale = FVector(
	FMath::Clamp(
		m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
		m_stem_ScaleRange_x_y.X,
		m_stem_ScaleRange_x_y.Y
	),
		FMath::Clamp(
			m_stem_ScaleRange_x_y.X + (m_stem_ScaleRange_x_y.Y - m_stem_ScaleRange_x_y.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
			m_stem_ScaleRange_x_y.X,
			m_stem_ScaleRange_x_y.Y
		),
		FMath::Clamp(
		  m_stem_ScaleRange_z.X + (m_stem_ScaleRange_z.Y - m_stem_ScaleRange_z.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
		m_stem_ScaleRange_z.X,
		m_stem_ScaleRange_z.Y
		)
	);

	CornStem->SetWorldScale3D(NewScale);

	UMaterialInterface* BaseMaterial = CornStem->GetMaterial(0);
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	if (DynMaterial)
	{
		// Apply slight individual variations
		DynMaterial->SetScalarParameterValue(FName("Hue"), (1 - plantAge) * m_stem_HueVariation + FMath::FRandRange(-0.05, 0.05));
		DynMaterial->SetScalarParameterValue(FName("Value"), (1 - plantAge) * m_stem_SaturationVariation + FMath::FRandRange(-0.05, 0.05));
		DynMaterial->SetScalarParameterValue(FName("Saturation"), (1 - plantAge) * m_stem_ValueVariation + FMath::FRandRange(-0.05, 0.05));
		CornStem->SetMaterial(0, DynMaterial);
	}
	
	// Get the stem's local bounds (without world transform)
	FVector stemOrigin, stemExtents;
	CornStem->GetLocalBounds(stemOrigin, stemExtents); // This gives the local bounds of the mesh

	// Calculate the height based on the local bounds' Z extent
	float stemHeight = stemExtents.Z * CornStem->GetComponentTransform().GetScale3D().Z;

	FVector2d zPositionRange(0.0, stemHeight);

	// Create leaves and position them along the stem
	int numLeaves = FMath::FloorToInt(FMath::RandRange(4, 8) * (stemHeight / 65));

	numLeaves = FMath::FloorToInt(numLeaves * leafDensity);

	float zStep = (zPositionRange.Y - zPositionRange.X) / (numLeaves / 2 + 1);

	for (int j = 0; j < numLeaves; j++)
	{
		
		UStaticMesh* RandomLeaf = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Corn/Leaves/");

		UStaticMeshComponent* LeafComp = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("Leaf_%d"), j)));
		LeafComp->SetMobility(EComponentMobility::Movable);
		LeafComp->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepRelativeTransform); 
		LeafComp->RegisterComponent();
		LeafComp->SetStaticMesh(RandomLeaf);
		CornLeaves.Add(LeafComp); 

		NewScale = FVector(
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		),
		FMath::Clamp(
			m_leaf_ScaleRange.X + (m_leaf_ScaleRange.Y - m_leaf_ScaleRange.X) * FMath::Sqrt(plantAge) + FMath::RandRange(-0.1f, 0.1f),
			m_leaf_ScaleRange.X,
			m_leaf_ScaleRange.Y
		)
	);

		LeafComp->SetWorldScale3D(NewScale);

		FMatrix TransformMatrix = LeafComp->GetComponentTransform().ToMatrixWithScale();
		FVector Shear = FVector(
			FMath::RandRange(m_leaf_ShearFactorRange_x_y.X, m_leaf_ShearFactorRange_x_y.Y),
			FMath::RandRange(m_leaf_ShearFactorRange_x_y.X, m_leaf_ShearFactorRange_x_y.Y),
			FMath::Clamp(
				m_leaf_ShearFactorRange_z.X + (m_leaf_ShearFactorRange_z.Y - m_leaf_ShearFactorRange_z.X) * plantAge + FMath::RandRange(-0.1f, 0.1f),
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

		LeafComp->SetWorldTransform(Transform);

		BaseMaterial = LeafComp->GetMaterial(0);
		DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (DynMaterial)
		{
			// Apply slight individual variations
			DynMaterial->SetScalarParameterValue(FName("Hue"), (1 - plantAge) * m_leaf_HueVariation + FMath::FRandRange(-0.05, 0.05));
			DynMaterial->SetScalarParameterValue(FName("Value"), (1 - plantAge) * m_leaf_SaturationVariation + FMath::FRandRange(-0.05, 0.05));
			DynMaterial->SetScalarParameterValue(FName("Saturation"), (1 - plantAge) * m_leaf_ValueVariation + FMath::FRandRange(-0.05, 0.05));
			LeafComp->SetMaterial(0, DynMaterial);
		}

		// Set relative position for each leaf
		FVector3d relativePos(0.0f, 0.0f, zPositionRange.X + (FMath::Floor(j / 2) + 1) * zStep);
		LeafComp->SetRelativeLocation(relativePos);

		// Apply random rotation (±180 degrees)
		float rotationAngle = j * (180.0f + FMath::RandRange(-10.0f, 10.0f));
		LeafComp->SetRelativeRotation(FRotator(0.0f, rotationAngle, 0.0f));
	}
}

