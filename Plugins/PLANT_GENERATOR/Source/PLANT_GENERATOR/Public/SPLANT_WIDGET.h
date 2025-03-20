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
	float SliderValue1 = 0.5f;  // Default value
	float SliderValue2 = 0.5f;
	int amount = 0;

	TSharedPtr<FString> SelectedOption;
	TArray<TSharedPtr<FString>> Options;

	FText GetSelectedOptionText() const;
	void OnSlider1Changed(float Value);
	void OnSlider2Changed(float Value);
	void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	void OnAmountChanged(int value);
	FReply OnGenerateClicked();

	Corn_generator corn_generator;
};
