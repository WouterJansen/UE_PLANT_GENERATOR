// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "PLANT_GENERATORStyle.h"

class FPLANT_GENERATORCommands : public TCommands<FPLANT_GENERATORCommands>
{
public:

	FPLANT_GENERATORCommands()
		: TCommands<FPLANT_GENERATORCommands>(TEXT("PLANT_GENERATOR"), NSLOCTEXT("Contexts", "PLANT_GENERATOR", "PLANT_GENERATOR Plugin"), NAME_None, FPLANT_GENERATORStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};