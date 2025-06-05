#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Plant_types.h"
#include "Crop_generator.generated.h"

UCLASS()
class PLANT_GENERATOR_API UCrop_Generator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Generators")
	static AActor* Create_variation(Plant_types GeneratorType, FTransform Location, TMap<FString, float> parameters);
	
};