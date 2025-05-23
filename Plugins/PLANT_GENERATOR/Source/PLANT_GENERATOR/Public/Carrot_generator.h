#pragma once
#include "CoreMinimal.h"

class PLANT_GENERATOR_API Carrot_generator
{
public: 
	Carrot_generator();
	~Carrot_generator();

	AActor* CreateVariation(TMap<FString, float> parameters, FTransform startTransform);

private:
	float m_carrot_MinHueVariation;
	float m_carrot_MaxHueVariation;
	float m_carrot_SaturationVariation;
	float m_carrot_ValueVariation;

	FVector2d m_carrot_ScaleRangeXZ;
	FVector2d m_carrot_ScaleRangeY;
	FVector2D m_carrot_ShearRange;
};
