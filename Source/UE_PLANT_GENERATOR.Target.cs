// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UE_PLANT_GENERATORTarget : TargetRules
{
	public UE_PLANT_GENERATORTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		ExtraModuleNames.Add("UE_PLANT_GENERATOR");
	}
}
