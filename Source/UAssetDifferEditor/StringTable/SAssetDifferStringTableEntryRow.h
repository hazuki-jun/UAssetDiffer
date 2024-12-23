// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDifferStringTableEntry.h"
#include "Widgets/Views/STableRow.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferStringTableEntryRow : public SMultiColumnTableRow<TSharedPtr<FCachedStringTableEntry>>
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferStringTableEntryRow) {}
		SLATE_ARGUMENT(bool, IsLocal)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<FCachedStringTableEntry> InCachedStringTableEntry, TSharedPtr<class SAssetDifferStringTableListView> InStringTableListView);
	
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	TSharedRef<SWidget> MakeRowActionsMenu();

	void OnMenuActionCopyName();
	void OnMenuActionCopyValue();
	void OnMenuActionMerge();

protected:
	TSharedPtr<class SAssetDifferStringTableListView> StringTableListView;
	TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry;
	bool bIsLocal = true;
};
