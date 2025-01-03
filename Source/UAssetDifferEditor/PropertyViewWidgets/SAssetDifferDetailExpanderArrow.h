﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailExpanderArrow : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnExpanderClicked, bool)
	
	SLATE_BEGIN_ARGS(SAssetDifferDetailExpanderArrow) {}
		SLATE_EVENT(FOnExpanderClicked, OnExpanderClicked)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedPtr<class SAssetDifferDetailTableRowBase> RowData);

private:

	EVisibility GetExpanderVisibility() const;
	const FSlateBrush* GetExpanderImage() const;
	FReply HandleOnExpanderClicked();
	
private:
	bool bIsExpanded = false;
	TWeakPtr<class SAssetDifferDetailTableRowBase> Row;
	TSharedPtr<SButton> ExpanderArrow;
	FOnExpanderClicked OnExpanderClicked;
};
