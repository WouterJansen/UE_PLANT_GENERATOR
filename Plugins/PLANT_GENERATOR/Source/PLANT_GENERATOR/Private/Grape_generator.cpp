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

    // It's good practice to create a new root component if you plan to attach many children,
    // or ensure the default one is suitable. For a cluster, a USceneComponent as root is often best.
    USceneComponent* ClusterRootComponent = Cast<UStaticMeshComponent>(NewActor->GetRootComponent());
    NewActor->SetRootComponent(ClusterRootComponent);
    ClusterRootComponent->RegisterComponent();

    // --- Parameter Extraction ---

    // Number of grapes in the cluster
    const int NumGrapes = FMath::RandRange(parameters.FindRef("NumGrapesMin", 30.0f), parameters.FindRef("NumGrapesMax", 80.f));
    
    // Length of the main rachis (central stem)
    const float RachisLength = FMath::RandRange(parameters.FindRef("RachisLengthMin", 500.0f), parameters.FindRef("RachisLengthMax", 1000.0f)); 
    
    // Length of the pedicel (stem connecting grape to rachis)
    const float PedicelLengthMin = FMath::RandRange(parameters.FindRef("PedicelLengthMin", 50.f), parameters.FindRef("PedicelLengthMinMax", 100.f));
    const float PedicelLengthMax = FMath::RandRange(parameters.FindRef("PedicelLengthMaxMin", 80.f), parameters.FindRef("PedicelLengthMaxMax", 140.f));

    TArray<FVector> GrapeFinalPositions;

    // 1. Define Main Rachis (Local Space)
    // Rachis goes from (0,0,0) downwards along -Z axis in local space of the ClusterRootComponent
    const FVector RachisStart = FVector::ZeroVector;
    const FVector RachisDirection = FVector(0, 0, -1); // Downwards
    
    USplineComponent* RachisSpline = NewObject<USplineComponent>(NewActor);
    RachisSpline->RegisterComponent();
    RachisSpline->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
    RachisSpline->SetMobility(EComponentMobility::Movable);

    FVector RachisEnd = RachisStart + RachisDirection * RachisLength;

    RachisSpline->SetSplinePoints({ RachisStart, RachisEnd }, ESplineCoordinateSpace::Local);
    RachisSpline->SetClosedLoop(false);

    UStaticMesh* RachisMesh =  Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/cylinders"));
    USplineMeshComponent* RachisSplineMesh = NewObject<USplineMeshComponent>(NewActor);
    RachisSplineMesh->RegisterComponent();
    RachisSplineMesh->SetMobility(EComponentMobility::Movable);

    RachisSplineMesh->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

    FVector RStart, RTangent, REnd, RTangentEnd;
    RachisSpline->GetLocationAndTangentAtSplinePoint(0, RStart, RTangent, ESplineCoordinateSpace::Local);
    RachisSpline->GetLocationAndTangentAtSplinePoint(1, REnd, RTangentEnd, ESplineCoordinateSpace::Local);

    RachisSplineMesh->SetStaticMesh(RachisMesh);
    RachisSplineMesh->SetStartAndEnd(RStart, RTangent, REnd, RTangentEnd);

    // Store initial positions before relaxation
    TArray<FVector> InitialGrapePositions;

    // Phyllotaxis constant (Golden Angle)
    const float GoldenAngle = 137.5f; // Degrees
    
    USplineComponent* PedicelSpline;
    USplineMeshComponent* PedicelMesh;
    UStaticMesh* StemMesh = Util::GetRandomMeshFromFolder("/PLANT_GENERATOR/Grape/cylinders");

    for (int i = 0; i < NumGrapes; ++i)
    {
        // 2. Determine Pedicel Attachment Point on Rachis
        // Distribute attachment points along the rachis.
        // We can use a slight curve or distribution, but linear is simplest to start.
        // Using a square root distribution to make grapes denser at the top (wider part) of the cluster
        float t = (float)i / (float)(NumGrapes -1 + KINDA_SMALL_NUMBER); // Normalized position along rachis [0,1]
        float RachisPointT = FMath::Pow(t, 1.0f); // Makes points denser towards start of rachis (t=0)
        FVector AttachmentPointOnRachis = RachisStart + RachisDirection * RachisPointT * RachisLength;

        // 3. Generate Pedicels
        float PedicelLength = FMath::RandRange(PedicelLengthMin, PedicelLengthMax) * (2-RachisPointT) ;

        // Pedicel direction: outwards and downwards relative to rachis.
        // Use Golden Angle for radial distribution around the rachis.
        float RadialAngle = (float)i * GoldenAngle;
        
        // Let's use UKismetMathLibrary::GetRandomUnitVectorInConeDegrees for a more robust approach
        // The cone should be oriented away from the rachis direction at the attachment point.
        // For simplicity, let's assume the rachis is mostly vertical.
        // We want pedicels to point generally away from RachisDirection and slightly down.
        // Create a vector perpendicular to rachis for the cone's main axis
        FVector OrthoRachis = FVector(FMath::Cos(FMath::DegreesToRadians(RadialAngle)), FMath::Sin(FMath::DegreesToRadians(RadialAngle)), 0);
        // Tilt this ortho vector slightly downwards
        float DownTilt = FMath::RandRange(0.1f, 0.5f); // Mix with RachisDirection
        FVector ConeAxis = (OrthoRachis * (1.0f - DownTilt) + RachisDirection * DownTilt).GetSafeNormal();
        float ConeHalfAngle = 15.0f; // Small cone for pedicel direction variation
        FVector PedicelDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ConeAxis, ConeHalfAngle);

        // 4. Place Berries (Initial Position)
        FVector GrapePos = AttachmentPointOnRachis + PedicelDir * PedicelLength;
        InitialGrapePositions.Add(GrapePos);

        // Create spline for pedicel
        PedicelSpline = NewObject<USplineComponent>(NewActor);
        PedicelSpline->RegisterComponent();
        PedicelSpline->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        PedicelSpline->SetMobility(EComponentMobility::Movable);

        PedicelSpline->SetSplinePoints({GrapePos, AttachmentPointOnRachis}, ESplineCoordinateSpace::Local);
        UE_LOG(LogTemp, Warning, TEXT("GrapePos: X=%f Y=%f Z=%f"), GrapePos.X, GrapePos.Y, GrapePos.Z);
        UE_LOG(LogTemp, Warning, TEXT("AttachmentPointOnRachis: X=%f Y=%f Z=%f"), AttachmentPointOnRachis.X, AttachmentPointOnRachis.Y, AttachmentPointOnRachis.Z);

        PedicelSpline->SetClosedLoop(false);

        // Create a spline mesh component for the pedicel
        PedicelMesh = NewObject<USplineMeshComponent>(NewActor);
        PedicelMesh->RegisterComponent();
        PedicelMesh->SetMobility(EComponentMobility::Movable);
        PedicelMesh->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        
        // Set start and end positions and tangents for the spline mesh
        FVector StartPos, StartTangent, EndPos, EndTangent;
        PedicelSpline->GetLocationAndTangentAtSplinePoint(0, StartPos, StartTangent, ESplineCoordinateSpace::Local);
        PedicelSpline->GetLocationAndTangentAtSplinePoint(1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

        PedicelMesh->SetStaticMesh(StemMesh);

        PedicelMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
    }

    // Make a copy for the relaxation process
    GrapeFinalPositions = InitialGrapePositions;

    // Create grape mesh components at generated positions
    for (int i = 0; i < GrapeFinalPositions.Num(); i++)
    {
       UStaticMesh* RandomGrapeMesh = Util::GetRandomMeshFromFolder(TEXT("/PLANT_GENERATOR/Grape/"));

       UStaticMeshComponent* GrapeComp = NewObject<UStaticMeshComponent>(NewActor);
        
       if (GrapeComp)
       {
           GrapeComp->SetStaticMesh(RandomGrapeMesh);
           GrapeComp->RegisterComponent();
           // Attach to the ClusterRootComponent we created for the actor
           GrapeComp->AttachToComponent(ClusterRootComponent, FAttachmentTransformRules::KeepRelativeTransform);

           FTransform LocalTransform;
           LocalTransform.SetLocation(GrapeFinalPositions[i]);
           // Optionally, add random rotation and scale variation to each grape
           float RandomScale = FMath::RandRange(0.9f, 1.1f);
           LocalTransform.SetScale3D(FVector(RandomScale)); // Scale based on its generated radius relative to max
           
            // --- NEW ROTATION LOGIC ---
           FVector GrapePosition = GrapeFinalPositions[i];
           
           // 1. Find the closest point on the rachis to the grape
           // Project the grape's position onto the rachis line (defined by RachisStart and RachisDirection)
           FVector PointOnRachis = FMath::ClosestPointOnLine(RachisStart, RachisDirection * RachisLength, GrapePosition) + FVector(0, 0, 50);
           
           // 2. Determine the direction from the grape to the rachis point
           // This vector points from the grape *towards* the rachis.
           FVector DirectionToRachis = (PointOnRachis - GrapePosition).GetSafeNormal();
           
           FQuat TargetQuat = FQuat::FindBetweenVectors(FVector(0,0,1), DirectionToRachis); // Align mesh's Z with DirectionToRachis

           // Add a small random twist around the rachis direction (like grapes naturally jostle)
           float RandomYaw = FMath::RandRange(-30.f, 30.f); // Random +/- 30 degrees
           FQuat RandomTwistQuat = FQuat(DirectionToRachis, FMath::DegreesToRadians(RandomYaw));
           
           LocalTransform.SetRotation(RandomTwistQuat * TargetQuat); // Apply twist *after* aligning Z

           GrapeComp->SetRelativeTransform(LocalTransform);
       }
    }

    return NewActor;
}
