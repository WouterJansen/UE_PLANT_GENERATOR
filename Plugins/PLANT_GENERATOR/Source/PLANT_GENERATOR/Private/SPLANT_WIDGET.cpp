#include "SPLANT_WIDGET.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSlider.h"
#include "Corn_generator.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPLANT_WIDGET::Construct(const FArguments& InArgs)
{
    // Populate dropdown options
    Options.Add(MakeShared<FString>("Corn"));
    Options.Add(MakeShared<FString>("Carrot"));
    Options.Add(MakeShared<FString>("Grape"));

    // Default selected option
    SelectedOption = Options[0];

    ChildSlot
    [
        SNew(SVerticalBox)

        // Dropdown selection
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Select Crop:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SComboBox<TSharedPtr<FString>>)
                .OptionsSource(&Options)
                .OnSelectionChanged(this, &SPLANT_WIDGET::OnSelectionChanged)
                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item)
                {
                    return SNew(STextBlock).Text(FText::FromString(*Item));
                })
                .InitiallySelectedItem(SelectedOption)
                [
                    SNew(STextBlock)
                    .Text(this, &SPLANT_WIDGET::GetSelectedOptionText)
                ]
            ]
        ]

        // Slider 1
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Slider 1:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSlider)
                .Value(this->SliderValue1)
                .OnValueChanged(this, &SPLANT_WIDGET::OnSlider1Changed)
            ]
        ]

        // Slider 2
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Slider 2:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSlider)
                .Value(this->SliderValue2)
                .OnValueChanged(this, &SPLANT_WIDGET::OnSlider2Changed)
            ]
        ]

        // Generate button
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Generate"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SButton)
                .OnClicked(this, &SPLANT_WIDGET::OnGenerateClicked)
            ]
        ]
    ];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

// Dropdown selection callback
void SPLANT_WIDGET::OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
    SelectedOption = NewValue;
}

// Get selected dropdown text
FText SPLANT_WIDGET::GetSelectedOptionText() const
{
    return FText::FromString(*SelectedOption);
}

// Slider callbacks
void SPLANT_WIDGET::OnSlider1Changed(float Value)
{
    SliderValue1 = Value;
}

void SPLANT_WIDGET::OnSlider2Changed(float Value)
{
    SliderValue2 = Value;
}

FReply SPLANT_WIDGET::OnGenerateClicked()
{
    Corn_generator::CreateVariations();
    return FReply::Handled();
}

