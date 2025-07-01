// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLANT_GENERATORCommands.h"

#define LOCTEXT_NAMESPACE "FPLANT_GENERATORModule"

void FPLANT_GENERATORCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "PLANT_GENERATOR", "Bring up PLANT_GENERATOR window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
