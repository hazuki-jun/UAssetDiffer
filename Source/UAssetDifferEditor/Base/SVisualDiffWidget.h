// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAssetDifferEditor/AssetDifferDefinitions.h"
#include "Widgets/SCompoundWidget.h"


/**
 * 
 */
class UASSETDIFFEREDITOR_API SVisualDiffWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SVisualDiffWidget) {}

	SLATE_END_ARGS()
	
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
	virtual TSharedRef<SWidget> MakeToolbar();
	
	virtual FText GetFilterText() const;
	virtual void OnFilterTextChanged(const FText& InFilterText);
	virtual void OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);
	virtual void UpdateVisibleRows();
	
	//~ Begin Toolbar Action Interface
	// Next
	virtual void ToolbarAction_HighlightNextDifference();
	// Prev
	virtual void ToolbarAction_HighlightPrevDifference();
	// Diff
	virtual bool IsDiffable();
	virtual void ToolbarAction_Diff();
	virtual bool ToolbarAction_CanDiff();
	// Merge
	virtual void ToolbarAction_Merge();
	virtual bool IsSearchable();
	//~ End Toolbar Action Interface
	
	// ViewOption Content
	virtual TSharedRef<SWidget> GetShowViewOptionContent();

	/** Called when show only view option is clicked */
	virtual void OnShowOnlyViewOptionClicked(EDifferRowViewOption InViewOption);
	virtual void RefreshLayout();
	virtual bool HasRowViewOption(EDifferRowViewOption InViewOption) const;
	virtual void ClearRowViewOption(EDifferRowViewOption InViewOption);
	virtual void SetRowViewOption(EDifferRowViewOption InViewOption);
	virtual void SetRowViewOptionTo(EDifferRowViewOption InViewOption);
	virtual void ReverseRowViewOption(EDifferRowViewOption InViewOption);
	virtual void ModifyConfig();
	
	// Modified Check State
	virtual bool IsShowOnlyRowViewOptionChecked(EDifferRowViewOption InViewOption) const;

protected:
	EDifferRowViewOption RowViewOption = EDifferRowViewOption::Max;
	FText ActiveFilterText;
	TSharedPtr<SSearchBox> SearchBoxWidget;
};
