#include "Crop_generator.h"
#include "Carrot_generator.h" 
#include "Corn_generator.h" 

TUniquePtr<Carrot_generator> UCrop_Generator::CarrotGenerator = MakeUnique<Carrot_generator>();
TUniquePtr<Corn_generator> UCrop_Generator::CornGenerator = MakeUnique<Corn_generator>();

void UCrop_Generator::Create_variations(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters)
{
	switch (GeneratorType)
	{
	case Plant_types::Carrot:
		if (CarrotGenerator.IsValid())
		{
			CarrotGenerator->CreateVariation(parameters, Location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CarrotGenerator is not initialized!"));
		}
		break;
	case Plant_types::Corn:
		if (CornGenerator.IsValid())
		{
			CornGenerator->CreateVariation(parameters, Location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CornGenerator is not initialized!"));
		}
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled GeneratorType: %d"), static_cast<uint8>(GeneratorType));
		break;
	}
}