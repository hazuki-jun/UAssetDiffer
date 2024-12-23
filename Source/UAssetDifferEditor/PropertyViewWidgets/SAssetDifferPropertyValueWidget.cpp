// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferPropertyValueWidget.h"

#include "SlateOptMacros.h"
#include "AssetDifferDetailTreeNode.h"


#define LOCTEXT_NAMESPACE "SAssetDifferPropertyValueWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferPropertyValueWidget::Construct(const FArguments& InArgs, TWeakPtr<FAssetDifferDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	if (!OwnerTreeNode.IsValid())
	{
		return;
	}
	auto Font = FCoreStyle::GetDefaultFontStyle("Regular", 8);
	ValueText = OwnerTreeNode.Pin()->GetValueText();
	if (ValueText.IsEmpty() || OwnerTreeNode.Pin()->IsContainerNode())
	{
		return;
	}
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.Padding(FMargin(10.f, 0.f, 10.f, 0.f))
			[
				SNew(SEditableTextBox)
				.IsReadOnly(true)
				.BackgroundColor(FSlateColor(FLinearColor(0.f, 0.f, 0.f)))
				.Text(this, &SAssetDifferPropertyValueWidget::GetText)
			]
		]
	];
}

void SAssetDifferPropertyValueWidget::Refresh()
{
	if (OwnerTreeNode.Pin().Get())
	{
		ValueText = OwnerTreeNode.Pin()->GetValueText();
	}
}

FText SAssetDifferPropertyValueWidget::GetText() const
{
	return ValueText;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
