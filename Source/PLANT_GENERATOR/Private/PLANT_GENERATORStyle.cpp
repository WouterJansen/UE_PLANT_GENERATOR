// Copyright Epic Games, Inc. All Rights Reserved.

#include "PLANT_GENERATORStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FPLANT_GENERATORStyle::StyleInstance = nullptr;

void FPLANT_GENERATORStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FPLANT_GENERATORStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FPLANT_GENERATORStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PLANT_GENERATORStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FPLANT_GENERATORStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PLANT_GENERATORStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PLANT_GENERATOR")->GetBaseDir() / TEXT("Resources"));

	Style->Set("PLANT_GENERATOR.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	return Style;
}

void FPLANT_GENERATORStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FPLANT_GENERATORStyle::Get()
{
	return *StyleInstance;
}
