#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Corn_generator.h"

class SPLANT_WIDGET : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPLANT_WIDGET) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	float plantage = 0.5f;  
	int amount = 0;

	TSharedPtr<FString> SelectedOption;
	TArray<TSharedPtr<FString>> Options;
	FString ExportPath;

	FText GetSelectedOptionText() const;
	void OnPlantAgeChanged(float Value);
	void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void OnExportPathChanged(const FText& NewText, ETextCommit::Type CommitType);
	void OnAmountChanged(int value);
	FReply OnGenerateClicked();

	Corn_generator corn_generator;
};
