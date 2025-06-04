// Fill out your copyright notice in the Description page of Project Settings.


#include "Grape_generator.h"
#include "Util.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

Grape_generator::Grape_generator()
{
}

Grape_generator::~Grape_generator()
{
}

AActor* Grape_generator::CreateVariation(TMap<FString, float> parameters, FTransform startTransform)
{

    AStaticMeshActor* NewActor = GWorld->SpawnActor<AStaticMeshActor>(startTransform.GetLocation(), startTransform.GetRotation().Rotator());
    USceneComponent* ClusterRootComponent = Cast<UStaticMeshComponent>(NewActor->GetRootComponent());
    NewActor->SetRootComponent(ClusterRootComponent);
    ClusterRootComponent->RegisterComponent();
    
    // Number of grapes in the cluster
    // const int NumGrapes = FMath::RandRange(parameters.FindRef("NumGrapesMin", 80.0f), parameters.FindRef("NumGrapesMax", 120.f));
    
    // Length of the main rachis (central stem)
    const float RachisLength = FMath::RandRange(parameters.FindRef("RachisLengthMin", 500.0f), parameters.FindRef("RachisLengthMax", 1100.0f)); 
    
    // Length of the pedicel (stem connecting grape to rachis)
    const float PedicelLengthMin = FMath::RandRange(parameters.FindRef("PedicelLengthMin", 25.f), parameters.FindRef("PedicelLengthMinMax", 100.f));
    const float PedicelLengthMax = FMath::RandRange(parameters.FindRef("PedicelLengthMaxMin", 100.f), parameters.FindRef("PedicelLengthMaxMax", 200.f));
    
    // Rachis bending parameters
    const int RachisSegments = FMath::RandRange(parameters.FindRef("RachisSegmentsMin", 3.0f), parameters.FindRef("RachisSegmentsMax", 6.0f));
    const float RachisBendMagnitude = FMath::RandRange(parameters.FindRef("RachisBendMagnitudeMin", 150.0f), parameters.FindRef("RachisBendMagnitudeMax", 1000.0f));

    // General direction of bend
    const FVector RachisBendAxis = FVector(
        FMath::RandRange(parameters.FindRef("RachisBendAxisXmin", 0.0f), parameters.FindRef("RachisBendAxisXmax", 1.0f)),
        FMath::RandRange(parameters.FindRef("RachisBendAxisYmin", 0.0f), parameters.FindRef("RachisBendAxisYmax", 1.0f)),
        FMath::RandRange(parameters.FindRef("RachisBendAxisZmin", 0.0f), parameters.FindRef("RachisBendAxisXmax", 1.0f))
    ).GetSafeNormal();

    TArray<FVector> GrapeFinalPositions;

    USplineComponent* RachisSpline = NewObject<USplineComponent>(NewActor);
    RachisSpline->RegisterComponent();
    RachisSpline->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    RachisSpline->SetMobility(EComponentMobility::Movable);

    TArray<FSplinePoint> RachisSplinePoints;
    const FVector RachisStart = FVector::ZeroVector;
    const FVector BaseRachisDirection = FVector(0, 0, -1); 

    // Generate intermediate spline points for bending
    for (int i = 0; i < RachisSegments; ++i)
    {
        float Alpha = (float)i / (float)(RachisSegments - 1);
        FVector PointLocation = RachisStart + BaseRachisDirection * RachisLength * Alpha;
        
        FVector BendOffset = RachisBendAxis * RachisBendMagnitude * FMath::RandRange(0.8f, 1.2f); 
        
        PointLocation += BendOffset;

        // Add randomness to each point's X/Y for a more organic look
        PointLocation.X += FMath::RandRange(-20.0f, 20.0f);
        PointLocation.Y += FMath::RandRange(-20.0f, 20.0f);

        RachisSplinePoints.Add(FSplinePoint(i, PointLocation));
    }
    
    RachisSpline->ClearSplinePoints();
    RachisSpline->AddPoints(RachisSplinePoints, false);
    RachisSpline->UpdateSpline(); 
    RachisSpline->SetClosedLoop(false);

    UStaticMesh* RachisMesh =  Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/cylinders/beam"));
    
    // Create multiple SplineMeshComponents for the rachis segments
    for (int i = 0; i < RachisSpline->GetNumberOfSplinePoints() - 1; ++i)
    {
        USplineMeshComponent* RachisSplineMesh = NewObject<USplineMeshComponent>(NewActor);
        RachisSplineMesh->RegisterComponent();
        RachisSplineMesh->SetMobility(EComponentMobility::Movable);
        RachisSplineMesh->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

        FVector RStart, RTangent, REnd, RTangentEnd;
        RachisSpline->GetLocationAndTangentAtSplinePoint(i, RStart, RTangent, ESplineCoordinateSpace::Local);
        RachisSpline->GetLocationAndTangentAtSplinePoint(i + 1, REnd, RTangentEnd, ESplineCoordinateSpace::Local);

        RachisSplineMesh->SetStaticMesh(RachisMesh);
        RachisSplineMesh->SetStartAndEnd(RStart, RTangent, REnd, RTangentEnd);
        RachisSplineMesh->SetForwardAxis(ESplineMeshAxis::X);
    }

    // Store initial positions
    TArray<FVector> InitialGrapePositions;
    TArray<FVector> InitialConnectionPositions;
    
    // Phyllotaxis constant (Golden Angle)
    const float GoldenAngle = 137.5f; // Degrees
    
    USplineComponent* PedicelSpline;
    USplineMeshComponent* PedicelMesh;
    UStaticMesh* StemMesh = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Grape/cylinders/small_beam");

    // Scale number of grapes based on rachis length
    int NumGrapes = RachisLength * PedicelLengthMax / 1000;

    for (int i = 0; i < NumGrapes; ++i)
    {
        // Distribute attachment points along the rachis using its length
        float RachisLengthAtPoint = FMath::Pow((float)i / (float)(NumGrapes - 1 + KINDA_SMALL_NUMBER), 2.5) * RachisSpline->GetSplineLength();
        
        FVector AttachmentPointOnRachis = RachisSpline->GetLocationAtDistanceAlongSpline(RachisLengthAtPoint, ESplineCoordinateSpace::Local);
        FVector RachisTangent = RachisSpline->GetTangentAtDistanceAlongSpline(RachisLengthAtPoint, ESplineCoordinateSpace::Local);
        RachisTangent.Normalize();
        
        // Adjust pedicel length based on distance along rachis (shorter at bottom)
        float PedicelLengthRatio = 1.0f - (RachisLengthAtPoint / RachisSpline->GetSplineLength()); 
        float PedicelLength = FMath::RandRange(PedicelLengthMin, PedicelLengthMax) * (0.5f + PedicelLengthRatio * 1.5f); 

        float RadialAngle = (float)i * GoldenAngle;
        
        // Generate a perpendicular vector to the RachisTangent
        FVector OrthoPerpendicular = FVector::CrossProduct(RachisTangent, FVector(0,0,1));
        if (OrthoPerpendicular.IsNearlyZero()) // If RachisTangent is aligned with Z
        {
            OrthoPerpendicular = FVector::CrossProduct(RachisTangent, FVector(0,1,0)); // Cross with Y-forward
        }
        OrthoPerpendicular.Normalize();

        // Rotate the perpendicular vector around the RachisTangent by RadialAngle
        FVector RotatedOrtho = OrthoPerpendicular.RotateAngleAxis(RadialAngle, RachisTangent);
        
        // Tilt the pedicel direction downwards and slightly outwards
        // The more towards the bottom of the cluster, the more vertical they should become
        float DownTiltWeight = FMath::Lerp(0.8f, 0.5f, RachisLengthAtPoint / RachisSpline->GetSplineLength());
        
        FVector PedicelDir = FMath::Lerp(FVector(0,0,-1), RotatedOrtho , DownTiltWeight).GetSafeNormal();

        // Add a small cone variation
        float ConeHalfAngle = 10.0f; 
        PedicelDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(PedicelDir, ConeHalfAngle);

        // 4. Place Berries (Initial Position)
        FVector GrapePos = AttachmentPointOnRachis + PedicelDir * PedicelLength;
        InitialGrapePositions.Add(GrapePos);
        InitialConnectionPositions.Add(AttachmentPointOnRachis);

        // Create spline for pedicel
        PedicelSpline = NewObject<USplineComponent>(NewActor);
        PedicelSpline->RegisterComponent();
        PedicelSpline->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        PedicelSpline->SetMobility(EComponentMobility::Movable);

        // Pedicel spline goes from grape to attachment point on rachis
        PedicelSpline->SetSplinePoints({GrapePos, AttachmentPointOnRachis}, ESplineCoordinateSpace::Local);
        PedicelSpline->SetClosedLoop(false);

        // Create a spline mesh component for the pedicel
        PedicelMesh = NewObject<USplineMeshComponent>(NewActor);
        PedicelMesh->RegisterComponent();
        PedicelMesh->SetMobility(EComponentMobility::Movable);
        PedicelMesh->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        
        // Set start and end positions and tangents for the spline mesh
        FVector PedicelStartPos, PedicelStartTangent, PedicelEndPos, PedicelEndTangent;
        PedicelSpline->GetLocationAndTangentAtSplinePoint(0, PedicelStartPos, PedicelStartTangent, ESplineCoordinateSpace::Local);
        PedicelSpline->GetLocationAndTangentAtSplinePoint(1, PedicelEndPos, PedicelEndTangent, ESplineCoordinateSpace::Local);

        PedicelMesh->SetStaticMesh(StemMesh);
        PedicelMesh->SetStartAndEnd(PedicelStartPos, PedicelStartTangent, PedicelEndPos, PedicelEndTangent);
    }
    
    // Apply random hue, saturation, value variation to the dynamic material
    float Hue = FMath::RandBool() ? 0.0f : 0.2f;
    float Value = FMath::FRandRange(0.f, 0);
    float Saturation = 0.0f;
    if (Hue == 0.f)
        Saturation = FMath::FRandRange(-0.5f, 0.1);
    else
    {
        Saturation = FMath::FRandRange(-0.1f, 0.1);
    }

    // Create grape mesh components at generated positions
    for (int i = 0; i < InitialGrapePositions.Num(); i++)
    {
       UStaticMesh* RandomGrapeMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/"));

       UStaticMeshComponent* GrapeComp = NewObject<UStaticMeshComponent>(NewActor);
        
       if (GrapeComp)
       {
           GrapeComp->SetStaticMesh(RandomGrapeMesh);
           GrapeComp->RegisterComponent();
           GrapeComp->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

           FTransform LocalTransform;
           FVector GrapePosition = InitialGrapePositions[i];
           LocalTransform.SetLocation(GrapePosition);
           float RandomScale = FMath::RandRange(0.8f, 1.2f);
           LocalTransform.SetScale3D(FVector(RandomScale));

           FVector AttachmentPointOnRachis = InitialConnectionPositions[i];
           FVector DirectionToRachis = (AttachmentPointOnRachis - GrapePosition).GetSafeNormal();
           
           // Align the grape's Z-axis with the DirectionToRachis
           FQuat TargetQuat = FQuat::FindBetweenVectors(FVector(0,0,1), DirectionToRachis); 

           // Add a small random twist around the DirectionToRachis (local Z)
           float RandomYaw = FMath::RandRange(-45.f, 45.f); 
           FQuat RandomTwistQuat = FQuat(DirectionToRachis, FMath::DegreesToRadians(RandomYaw));
           
           LocalTransform.SetRotation(RandomTwistQuat * TargetQuat); 

           GrapeComp->SetRelativeTransform(LocalTransform);
       }

        FString PackagePath = TEXT("/game/generated_materials");
        FString AssetName = TEXT("generated_material"); 

        UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(RandomGrapeMesh->GetMaterial(0), GetTransientPackage());
        DynMaterial->SetScalarParameterValue(FName("Hue"), Hue + FMath::FRandRange(-0.02f, 0.02f));
        DynMaterial->SetScalarParameterValue(FName("Value"), Value+ FMath::FRandRange(-0.01f, 0.01f));
        DynMaterial->SetScalarParameterValue(FName("Saturation"), Saturation + FMath::FRandRange(-0.1f, 0.1f));
        GrapeComp->SetMaterial(0, DynMaterial);
    }
    
    return NewActor;
}