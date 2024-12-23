// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDataTableDetailRowSelector.h"

#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "SAssetDifferDataTableDetailRowSelector"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDataTableDetailRowSelector::Construct(const FArguments& InArgs)
{
	for (const auto& Row : InArgs._Rows)
	{
		Rows.Add(MakeShared<FName>(Row));
	}
	
	SelectedRowText = FText::FromName(InArgs._SelectedRowName);
	
	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(170)
		.ToolTipText(LOCTEXT("SelectedRowTooltip", "Select a row to diff"))
		[
			SAssignNew(MyComboBox, SComboBox<TSharedPtr<FName>>)
			.OptionsSource(&Rows)
			.OnSelectionChanged(this, &SAssetDifferDataTableDetailRowSelector::OnSelectionChanged)
			.OnGenerateWidget(this, &SAssetDifferDataTableDetailRowSelector::OnGenerateComboWidget)
			.Content()
			[
				SNew(STextBlock).Text(this, &SAssetDifferDataTableDetailRowSelector::GetCurrentName)
			]
		]
	];
}

void SAssetDifferDataTableDetailRowSelector::OnSelectionChanged(TSharedPtr<FName> InItem, ESelectInfo::Type InSelectionInfo)
{
	// SelectedRowText = FText::FromName(*InItem.Get());
}

TSharedRef<SWidget> SAssetDifferDataTableDetailRowSelector::OnGenerateComboWidget(TSharedPtr<FName> InItem)
{
	return SNew(STextBlock).Text(FText::FromName(InItem.IsValid() ? *InItem : NAME_None));
}

FText SAssetDifferDataTableDetailRowSelector::GetCurrentName() const
{
	return SelectedRowText;
}

void SAssetDifferDataTableDetailRowSelector::OnRowSelected(FName RowName)
{
	SelectedRowText = FText::FromName(RowName);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
