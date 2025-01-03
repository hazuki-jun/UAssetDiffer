﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDetailRowIndent.h"

#include "SlateOptMacros.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDetailRowIndent::Construct(const FArguments& InArgs, TSharedRef<SAssetDifferDetailTableRowBase> DetailsRow)
{
	Row = DetailsRow;

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(this, &SAssetDifferDetailRowIndent::GetIndentWidth)
	];
}

int32 SAssetDifferDetailRowIndent::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
#if ENGINE_MAJOR_VERSION == 4
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
#endif
	
	TSharedPtr<SAssetDifferDetailTableRowBase> RowPtr = Row.Pin();
	if (!RowPtr.IsValid())
	{
		return LayerId;
	}

	const FSlateBrush* BackgroundBrush = FAppStyle::GetBrush("DetailsView.CategoryMiddle");
	const FSlateBrush* DropShadowBrush = FAppStyle::GetBrush("DetailsView.ArrayDropShadow");

	int32 IndentLevel = RowPtr->GetIndentLevelForBackgroundColor();
	for (int32 i = 0; i < IndentLevel; ++i)
	{
		FSlateColor BackgroundColor = GetRowBackgroundColor(i);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(FVector2D(16 * i, 0), FSlateLayoutTransform(FVector2D(16, AllottedGeometry.GetLocalSize().Y))),
			BackgroundBrush,
			ESlateDrawEffect::None,
			BackgroundColor.GetColor(InWidgetStyle)
		);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2D(16 * i, 0), FSlateLayoutTransform(FVector2D(16, AllottedGeometry.GetLocalSize().Y))),
			DropShadowBrush
		);
	}
		
	return LayerId + 1;
}

FOptionalSize SAssetDifferDetailRowIndent::GetIndentWidth() const
{
	int32 IndentLevel = 0;

	TSharedPtr<SAssetDifferDetailTableRowBase> RowPtr = Row.Pin();
	if (RowPtr.IsValid())
	{
		IndentLevel = RowPtr->GetIndentLevelForBackgroundColor();
	}

	return IndentLevel * 16.0f;
}

FSlateColor SAssetDifferDetailRowIndent::GetRowBackgroundColor(int32 IndentLevel) const
{
	int32 ColorIndex = 0;
	int32 Increment = 1;

	for (int i = 0; i < IndentLevel; ++i)
	{
		ColorIndex += Increment;

		if (ColorIndex == 0 || ColorIndex == 3)
		{
			Increment = -Increment;
		}
	}

	static const uint8 ColorOffsets[] =
	{
		0, 4, (4 + 2), (6 + 4), (10 + 6)
	};

	const FSlateColor BaseSlateColor = IsHovered() ? 
		FAppStyle::Get().GetSlateColor("Colors.Header") : 
		FAppStyle::Get().GetSlateColor("Colors.Panel");

	const FColor BaseColor = BaseSlateColor.GetSpecifiedColor().ToFColor(true);

	const FColor ColorWithOffset(
		BaseColor.R + ColorOffsets[ColorIndex], 
		BaseColor.G + ColorOffsets[ColorIndex], 
		BaseColor.B + ColorOffsets[ColorIndex]);

	return FSlateColor(FLinearColor::FromSRGBColor(ColorWithOffset));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
