// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferPropertyValueWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferPropertyValueWidget)
		{}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TWeakPtr<class FAssetDifferDetailTreeNode> InOwnerTreeNode);

	FText GetText() const;

	void Refresh();
	
	TWeakPtr<class FAssetDifferDetailTreeNode> OwnerTreeNode;

protected:
	FText ValueText;
};



