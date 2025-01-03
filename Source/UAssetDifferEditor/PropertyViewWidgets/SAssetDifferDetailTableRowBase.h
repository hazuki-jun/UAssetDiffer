﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailTableRowBase : public STableRow< TSharedPtr< class FAssetDifferDetailTreeNode > >
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDetailTableRowBase) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Refresh() {}
	
	virtual void OnExpanderClicked(bool bIsExpanded) {}

	virtual int32 GetIndentLevelForBackgroundColor() { return IndentLevel; }
	
protected:
	int32 IndentLevel = 0;
};
