#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AppleActor.generated.h"

UCLASS()
class PLANT_GENERATOR_API AAppleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAppleActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Grape Cluster Generation") 
	void GenerateApple(TMap<FString, float> parameters);

};
