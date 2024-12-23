// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAssetDifferDetailTableRowBase.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailRowIndent : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDetailRowIndent){}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<SAssetDifferDetailTableRowBase> DetailsRow);

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	FOptionalSize GetIndentWidth() const;
	FSlateColor GetRowBackgroundColor(int32 IndentLevel) const;

private:
	TWeakPtr<SAssetDifferDetailTableRowBase> Row;
};
