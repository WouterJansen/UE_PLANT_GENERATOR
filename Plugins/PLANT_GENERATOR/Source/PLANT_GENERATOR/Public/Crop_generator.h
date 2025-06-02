#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Plant_types.h"
#include "Carrot_generator.h"
#include "Corn_generator.h"
#include "Grape_generator.h"
#include "Crop_generator.generated.h"

UCLASS()
class PLANT_GENERATOR_API UCrop_Generator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Generators")
	static AActor* Create_variations(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters);

private:
	static TUniquePtr<Carrot_generator> CarrotGenerator;
	static TUniquePtr<Corn_generator> CornGenerator;
	static TUniquePtr<Grape_generator> GrapeGenerator;
	
};