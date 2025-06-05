#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CornActor.generated.h"

UCLASS()
class PLANT_GENERATOR_API ACornActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACornActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corn")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corn")
	UStaticMeshComponent* CornStem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corn")
	TArray<UStaticMeshComponent*> CornLeaves;

	UFUNCTION(BlueprintCallable, Category = "Corn Generation") 
	void GenerateCorn(TMap<FString, float> parameters);

private:
	FVector2D m_leaf_ScaleRange;
	float m_leaf_HueVariation;
	float m_leaf_SaturationVariation; 
	float m_leaf_ValueVariation;
	FVector2D m_leaf_ShearFactorRange_x_y;
	FVector2D m_leaf_ShearFactorRange_z;

	FVector2D m_stem_ScaleRange_x_y;
	FVector2D m_stem_ScaleRange_z;
	float m_stem_HueVariation;
	float m_stem_SaturationVariation; 
	float m_stem_ValueVariation;

};
