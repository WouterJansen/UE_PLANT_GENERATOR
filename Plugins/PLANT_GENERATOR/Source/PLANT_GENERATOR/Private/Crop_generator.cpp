#include "Crop_generator.h"
#include "AGrapeClusterActor.h"
#include "ACarrotActor.h"

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
		return nullptr;
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