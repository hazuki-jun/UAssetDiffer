// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditorUtils.h"
#include "UAssetDifferEditor/AssetDifferDefinitions.h"
#include "Widgets/SCompoundWidget.h"

struct FAssetDifferDataTableRowListViewData
{
	/** Unique ID used to identify this row */
	FName RowId;

	/** Display name of this row */
	FText DisplayName;

	/** The calculated height of this row taking into account the cell data for each column */
	float DesiredRowHeight;

	/** Insertion number of the row */
	uint32 RowNum = 0;

	/** Array corresponding to each cell in this row */
	TArray<FText> CellData;

	EDifferRowViewOption RowState = EDifferRowViewOption::Normal;
};

typedef TSharedPtr<FAssetDifferDataTableRowListViewData> FAssetDifferDataTableRowListViewDataPtr;

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDataTableListViewRow : public SMultiColumnTableRow<FAssetDifferDataTableRowListViewDataPtr>
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDataTableListViewRow) {}
		SLATE_ARGUMENT(bool, IsLocal)
		SLATE_ARGUMENT(TSharedPtr<class SDataTableVisualDiff>, DataTableVisual)
		SLATE_ARGUMENT(TSharedPtr<class SAssetDifferDataTableLayout>, DataTableLayout)
		SLATE_ARGUMENT(FAssetDifferDataTableRowListViewDataPtr, InRowDataPtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	//~ Begin SMultiColumnTableRow Interface
	virtual const FSlateBrush* GetBorder() const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnId) override;
	//~ End SMultiColumnTableRow Interface

	FSlateColor HandleTextColorAndOpacity() const;
	
	TSharedRef<SWidget> MakeCellWidget(const FName& InColumnId);

	TSharedRef<SWidget> MakeRowActionsMenu();

	// 右键菜单 CopyName
	void OnMenuActionCopyName();

	// 右键菜单 CopyName
	void OnMenuActionCopyValue();

	// 右键菜单 ShowDifference
	void OnMenuActionShowDifference();

	// 右键菜单 Merge
	void OnMenuActionMerge();
	
	bool bIsLocal = true;
	FAssetDifferDataTableRowListViewDataPtr RowDataPtr;
	TSharedPtr<class SDataTableVisualDiff> DataTableVisual = nullptr;
	TSharedPtr<class SAssetDifferDataTableLayout> DataTableLayout;
	FName ColumnId;
};


