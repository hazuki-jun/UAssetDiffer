// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferPropertyNameWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferPropertyNameWidget) {}
		// SLATE_ARGUMENT(FText, DisplayNameText)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakPtr<class FAssetDifferDetailTreeNode> OwnerNode);
};
