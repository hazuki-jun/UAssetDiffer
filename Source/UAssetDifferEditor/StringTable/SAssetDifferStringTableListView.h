// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetDifferStringTableEntry.h"
#include "Widgets/SCompoundWidget.h"



/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferStringTableListView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferStringTableListView) {}
		SLATE_ARGUMENT(bool, IsLocal)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, TSharedPtr<class SStringTableVisualDiff> InStringTableVisualDiff);

	void Refresh();

	void FilterRows(const FString& FilterName);
	
	void SetupRowsListSources();

	void HighlightRow(const FString& RowKey);

	void MergeRow(const FString& RowKey);

	void SetScrollOffset(float InScrollOffset);
	
	void OnListViewScrolled(double InScrollOffset);
	
public:
	TArray<TSharedPtr<FCachedStringTableEntry>> CachedStringTableEntries;

protected:
	/** Create the row for the given cached string table entry */
	TSharedRef<ITableRow> OnGenerateStringTableEntryRow(TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry, const TSharedRef<STableViewBase>& Table);
	
protected:
	bool bIsLocal = true;
	
	TSharedPtr<class SStringTableVisualDiff> StringTableVisualDiff; 
	
	/** List view showing the cached string table entries */
	TSharedPtr<SListView<TSharedPtr<FCachedStringTableEntry>>> MyListView;
};




