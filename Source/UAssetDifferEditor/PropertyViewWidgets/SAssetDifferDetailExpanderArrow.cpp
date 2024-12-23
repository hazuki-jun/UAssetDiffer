// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDetailExpanderArrow.h"

#include "SlateOptMacros.h"
#include "SAssetDifferDetailTableRowBase.h"

#define LOCTEXT_NAMESPACE "SAssetDifferDetailExpanderArrow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDetailExpanderArrow::Construct(const FArguments& InArgs, TSharedPtr<class SAssetDifferDetailTableRowBase> RowData)
{
	Row = RowData;
	OnExpanderClicked = InArgs._OnExpanderClicked;
	ChildSlot
	[
	SNew(SBox)
		.Visibility(this, &SAssetDifferDetailExpanderArrow::GetExpanderVisibility)
		[
			SAssignNew(ExpanderArrow, SButton)
			.ButtonStyle(FCoreStyle::Get(), "NoBorder")
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.ClickMethod(EButtonClickMethod::MouseDown)
			.OnClicked(this, &SAssetDifferDetailExpanderArrow::HandleOnExpanderClicked)
			.ContentPadding(0)
			.IsFocusable(false)
			[
				SNew(SImage)
				.Image(this, &SAssetDifferDetailExpanderArrow::GetExpanderImage)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				// SNew(STextBlock)
				// .Text(FText::FromString(TEXT("Arrow")))
			]
		]
	];

}

EVisibility SAssetDifferDetailExpanderArrow::GetExpanderVisibility() const
{
	TSharedPtr<SAssetDifferDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return EVisibility::Hidden;
	}
	
	return RowPtr->DoesItemHaveChildren() ? EVisibility::Visible : EVisibility::Hidden;
}

const FSlateBrush* SAssetDifferDetailExpanderArrow::GetExpanderImage() const
{
	TSharedPtr<SAssetDifferDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return FAppStyle::GetBrush( "NoBrush" );
	}

	FName ResourceName;
	if (RowPtr->IsItemExpanded())
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName ExpandedHoveredName = "TreeArrow_Expanded_Hovered";
			ResourceName = ExpandedHoveredName;
		}
		else
		{
			static const FName ExpandedName = "TreeArrow_Expanded";
			ResourceName = ExpandedName;
		}
	}
	else
	{
		if (ExpanderArrow->IsHovered())
		{
			static const FName CollapsedHoveredName = "TreeArrow_Collapsed_Hovered";
			ResourceName = CollapsedHoveredName;
		}
		else
		{
			static const FName CollapsedName = "TreeArrow_Collapsed";
			ResourceName = CollapsedName;
		}
	}

	return FAppStyle::GetBrush( ResourceName);
}

FReply SAssetDifferDetailExpanderArrow::HandleOnExpanderClicked()
{
	TSharedPtr<SAssetDifferDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return FReply::Unhandled();
	}
	
	const FModifierKeysState ModKeyState = FSlateApplication::Get().GetModifierKeys();
	if (!ModKeyState.IsShiftDown())
	{
		RowPtr->ToggleExpansion();
	}
	OnExpanderClicked.ExecuteIfBound(RowPtr->IsItemExpanded());
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
