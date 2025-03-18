// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PLANT_GENERATOR_API Corn_generator
{
public:
	Corn_generator();
	~Corn_generator();

	static UStaticMesh* CreateVariations(FVector2D ScaleRange = FVector2D(0.25f, 4.0f),
						  float HueVariation = 0.15f, 
						  float SaturationVariation = 0.2f, 
						  float ValueVariation = 0.2f, 
						  FVector2D ShearFactorRange = FVector2D(-0.5f, 1.0f));
};
