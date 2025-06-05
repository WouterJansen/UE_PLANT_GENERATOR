#include "Crop_generator.h"
#include "GrapeClusterActor.h"
#include "CarrotActor.h"
#include "CornActor.h"

AActor* UCrop_Generator::Create_variation(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters)
{
	switch (GeneratorType)
	{
	case Plant_types::Carrot:
		{
			ACarrotActor* NewCarrot = GWorld->SpawnActor<ACarrotActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewCarrot)
			{
				NewCarrot->GenerateCarrot(parameters); 
			}
			return NewCarrot;
		}
	case Plant_types::Corn:
		{
			ACornActor* NewCorn = GWorld->SpawnActor<ACornActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewCorn)
			{
				NewCorn->GenerateCorn(parameters); 
			}
			return NewCorn;
		}
	case Plant_types::Grape:
		{
			AGrapeClusterActor* NewGrapeCluster = GWorld->SpawnActor<AGrapeClusterActor>(Location.GetLocation(), Location.GetRotation().Rotator());
			if (NewGrapeCluster)
			{
				NewGrapeCluster->GenerateGrapeCluster(parameters); 
			}
			return NewGrapeCluster; 
		}
	default:
		return nullptr;
	}
}