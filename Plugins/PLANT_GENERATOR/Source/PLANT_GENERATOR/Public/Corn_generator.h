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

	void CreateVariation();
	
private:

	FVector2D m_leaf_ScaleRange;
	float m_leaf_HueVariation;
	float m_leaf_SaturationVariation; 
	float m_leaf_ValueVariation;
	FVector2D m_leaf_ShearFactorRange;

	FVector2D m_stem_ScaleRange_x_y;
	FVector2D m_stem_ScaleRange_z;
	float m_stem_HueVariation;
	float m_stem_SaturationVariation; 
	float m_stem_ValueVariation;

	UStaticMeshComponent* CreateLeafVariation();
	UStaticMeshComponent* CreateStemVariation();
	void SpawnActor(TArray<UStaticMeshComponent*> Components, TArray<FVector3d> RelativePositions, FVector3d ActorLocation);
};
