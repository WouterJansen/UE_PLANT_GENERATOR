#include "Crop_generator.h"
#include "Carrot_generator.h" 
#include "Corn_generator.h"
#include "Grape_generator.h"

TUniquePtr<Carrot_generator> UCrop_Generator::CarrotGenerator = MakeUnique<Carrot_generator>();
TUniquePtr<Corn_generator> UCrop_Generator::CornGenerator = MakeUnique<Corn_generator>();
TUniquePtr<Grape_generator> UCrop_Generator::GrapeGenerator = MakeUnique<Grape_generator>();

AActor* UCrop_Generator::Create_variations(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters)
{
	switch (GeneratorType)
	{
	case Plant_types::Carrot:
		if (CarrotGenerator.IsValid())
		{
			return CarrotGenerator->CreateVariation(parameters, Location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CarrotGenerator is not initialized!"));
			return nullptr;
		}
		break;
	case Plant_types::Corn:
		if (CornGenerator.IsValid())
		{
			return CornGenerator->CreateVariation(parameters, Location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CornGenerator is not initialized!"));
			return nullptr;
		}
		break;
	case Plant_types::Grape:
		if (GrapeGenerator.IsValid())
		{
			return GrapeGenerator->CreateVariation(parameters, Location);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GrapeGenerator is not initialized!"));
			return nullptr;
		}
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Unhandled GeneratorType: %d"), static_cast<uint8>(GeneratorType));
		return nullptr;
		break;
	}
}