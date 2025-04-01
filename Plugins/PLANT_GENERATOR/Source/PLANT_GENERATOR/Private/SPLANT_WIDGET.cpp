#include "SPLANT_WIDGET.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSlider.h"
#include "Corn_generator.h"
#include "Widgets/Input/SSpinBox.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPLANT_WIDGET::Construct(const FArguments& InArgs)
{
    
    corn_generator = Corn_generator();
    
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
                .Text(FText::FromString("Plant age:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSlider)
                .Value(this->plantage)
                .OnValueChanged(this, &SPLANT_WIDGET::OnPlantAgeChanged)
            ]
        ]
        
        // Number of generations
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(FText::FromString("Amount:"))
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SSpinBox<int>)
                .Value(this->amount)
                .OnValueChanged(this, &SPLANT_WIDGET::OnAmountChanged)
            ]
        ]

        // Export Path Input
       + SVerticalBox::Slot()
       .AutoHeight()
       .Padding(5)
       [
           SNew(SHorizontalBox)
           + SHorizontalBox::Slot()
           .AutoWidth()
           [
               SNew(STextBlock)
               .Text(FText::FromString("Export Path:"))
           ]
           + SHorizontalBox::Slot()
           [
               SNew(SEditableTextBox)
               .Text(FText::FromString(*exportpath))
               .OnTextCommitted(this, &SPLANT_WIDGET::OnExportPathChanged)
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
void SPLANT_WIDGET::OnPlantAgeChanged(float Value)
{
    plantage = Value;
}


void SPLANT_WIDGET::OnAmountChanged(int value)
{
    amount = value;
}

FReply SPLANT_WIDGET::OnGenerateClicked()
{
    corn_generator.CreateVariation(amount, plantage, exportpath);
    return FReply::Handled();
}

// Export Path Callback
void SPLANT_WIDGET::OnExportPathChanged(const FText& NewText, ETextCommit::Type CommitType)
{
    exportpath = NewText.ToString();
}
