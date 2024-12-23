// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAssetDifferDetailTableRowBase.h"
/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailCategoryRow : public SAssetDifferDetailTableRowBase
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDetailCategoryRow) {}
		SLATE_ARGUMENT(FName, CategoryName)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TWeakPtr<class FAssetDifferDetailTreeNode> OwnerTreeNode_);
	virtual void OnExpanderClicked(bool bIsExpanded) override;

	virtual void Refresh() override;
	
protected:
	const FSlateBrush* GetBackgroundImage() const;
	FSlateColor GetInnerBackgroundColor() const;
	FSlateColor GetOuterBackgroundColor() const;
	FSlateColor GetDisplayNameColor() const;
	
	FSlateColor DisplayNameColor;
	TWeakPtr<class FAssetDifferDetailTreeNode> OwnerTreeNode;
};
