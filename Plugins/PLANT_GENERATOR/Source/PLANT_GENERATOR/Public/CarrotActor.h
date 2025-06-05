// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CarrotActor.generated.h"

UCLASS()
class PLANT_GENERATOR_API ACarrotActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	ACarrotActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Carrot")
	UStaticMeshComponent* Carrot;
	
	UFUNCTION(BlueprintCallable, Category = "Carrot Generation") 
	void GenerateCarrot(TMap<FString, float> parameters);
	
private:
	float m_carrot_MinHueVariation;
	float m_carrot_MaxHueVariation;
	float m_carrot_SaturationVariation;
	float m_carrot_ValueVariation;

	FVector2d m_carrot_ScaleRangeXZ;
	FVector2d m_carrot_ScaleRangeY;
	FVector2D m_carrot_ShearRange;
};
