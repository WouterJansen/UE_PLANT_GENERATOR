#include "SPLANT_WIDGET.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSlider.h"
#include "Corn_generator.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h" // Add this include for checkbox support

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

        // Dropdown selection - always shown
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

        // Corn-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(CornControls, SVerticalBox)
            // Only visible when Corn is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Corn" ? EVisibility::Visible : EVisibility::Collapsed;
            })

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
        ]

        // Carrot-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(CarrotControls, SVerticalBox)
            // Only visible when Carrot is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Carrot" ? EVisibility::Visible : EVisibility::Collapsed;
            })
            
            // Example carrot control
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
            
            // New checkbox for "Cracked" option
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Cracked:"))
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SCheckBox)
                    .IsChecked(this, &SPLANT_WIDGET::IsCrackedChecked)
                    .OnCheckStateChanged(this, &SPLANT_WIDGET::OnCrackedChanged)
                ]
            ]
        ]

        // Grape-specific controls
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(5)
        [
            SAssignNew(GrapeControls, SVerticalBox)
            // Only visible when Grape is selected
            .Visibility_Lambda([this]() {
                return *SelectedOption == "Grape" ? EVisibility::Visible : EVisibility::Collapsed;
            })
            
            // Example grape control
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(5)
            [
                SNew(STextBlock)
                .Text(FText::FromString("Grape-specific settings"))
            ]
        ]

        // Generate button - shown for all options
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

// Add these new functions to handle the checkbox state
ECheckBoxState SPLANT_WIDGET::IsCrackedChecked() const
{
    return cracked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SPLANT_WIDGET::OnCrackedChanged(ECheckBoxState NewState)
{
    cracked = (NewState == ECheckBoxState::Checked);
}

// Dropdown selection callback
void SPLANT_WIDGET::OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
    SelectedOption = NewValue;
    
    // Update visibility of control groups when selection changes
    if (CornControls.IsValid())
    {
        CornControls->SetVisibility(*SelectedOption == "Corn" ? EVisibility::Visible : EVisibility::Collapsed);
    }
    if (CarrotControls.IsValid())
    {
        CarrotControls->SetVisibility(*SelectedOption == "Carrot" ? EVisibility::Visible : EVisibility::Collapsed);
    }
    if (GrapeControls.IsValid())
    {
        GrapeControls->SetVisibility(*SelectedOption == "Grape" ? EVisibility::Visible : EVisibility::Collapsed);
    }
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
    if (*SelectedOption == "Corn")
    {
        corn_generator.CreateVariation(amount, plantage, exportpath);
    }
    else if (*SelectedOption == "Carrot")
    {
        carrot_generator.CreateVariation(amount, cracked); 
    }
    else if (*SelectedOption == "Grape")
    {
        // Handle grape generation
    }
    return FReply::Handled();
}