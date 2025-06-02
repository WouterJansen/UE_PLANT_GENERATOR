// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PLANT_GENERATOR_API Grape_generator
{
public:
	Grape_generator();
	~Grape_generator();

	AActor* CreateVariation(TMap<FString, float> parameters, FTransform startTransform);

};
