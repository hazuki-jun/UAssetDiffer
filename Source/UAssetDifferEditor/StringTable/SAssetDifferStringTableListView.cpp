// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferStringTableListView.h"

#include "SlateOptMacros.h"
#include "SStringTableVisualDiff.h"
#include "SAssetDifferStringTableEntryRow.h"
#include "Internationalization/StringTableCore.h"


#define LOCTEXT_NAMESPACE "SAssetDifferStringTableListView"

namespace NS_AssetDifferStringTableListView
{
	const FName StringTableDummyColumnId("Dummy");
	const FName StringTableKeyColumnId("Key");
	const FName StringTableSourceStringColumnId("SourceString");
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferStringTableListView::Construct(const FArguments& InArgs, TSharedPtr<class SStringTableVisualDiff> InStringTableVisualDiff)
{
	StringTableVisualDiff = InStringTableVisualDiff;
	bIsLocal = InArgs._IsLocal;
	SetupRowsListSources();
	ChildSlot
	[
		SAssignNew(MyListView, SListView<TSharedPtr<FCachedStringTableEntry>>)
				.OnListViewScrolled(this, &SAssetDifferStringTableListView::OnListViewScrolled)
				.ListItemsSource(&CachedStringTableEntries)
				.OnGenerateRow(this, &SAssetDifferStringTableListView::OnGenerateStringTableEntryRow)
				.HeaderRow(
					SNew(SHeaderRow)
						+SHeaderRow::Column(NS_AssetDifferStringTableListView::StringTableDummyColumnId)
						.DefaultLabel(FText::GetEmpty())
						.FixedWidth(20)

						+SHeaderRow::Column(NS_AssetDifferStringTableListView::StringTableKeyColumnId)
						.DefaultLabel(LOCTEXT("KeyColumnLabel", "Key"))
						.FillWidth(0.2f)

						+SHeaderRow::Column(NS_AssetDifferStringTableListView::StringTableSourceStringColumnId)
						.DefaultLabel(LOCTEXT("SourceStringColumnLabel", "Source String"))
						.FillWidth(1.0f)
				)
	];

}

void SAssetDifferStringTableListView::OnListViewScrolled(double InScrollOffset)
{
	if (StringTableVisualDiff)
	{
		StringTableVisualDiff->Sync_VerticalScrollOffset(bIsLocal, InScrollOffset);
	}
}

void SAssetDifferStringTableListView::Refresh()
{
	CachedStringTableEntries.Empty();
	SetupRowsListSources();
	MyListView->RequestListRefresh();
}

void SAssetDifferStringTableListView::FilterRows(const FString& FilterName)
{
	CachedStringTableEntries.Empty();
	SetupRowsListSources();

	if (!FilterName.IsEmpty())
	{
		CachedStringTableEntries.RemoveAll([&](const TSharedPtr<FCachedStringTableEntry>& Entry)
		{
			if (Entry->Key.Find(FilterName) == INDEX_NONE)
			{
				return true;
			}

			return false;
		});	
	}
	
	MyListView->RequestListRefresh();
}

void SAssetDifferStringTableListView::SetupRowsListSources()
{
	StringTableVisualDiff->GetStringTableData(bIsLocal, CachedStringTableEntries);
	
	CachedStringTableEntries.Sort([](const TSharedPtr<FCachedStringTableEntry>& InEntryOne, const TSharedPtr<FCachedStringTableEntry>& InEntryTwo)
	{
		return InEntryOne->Key < InEntryTwo->Key;
	});
	
	CachedStringTableEntries.RemoveAll([this](const TSharedPtr<FCachedStringTableEntry>& Entry)
	{
		if (StringTableVisualDiff->HasRowViewOption(static_cast<EDifferRowViewOption>(Entry->RowState)))
		{
			return false;
		}

		return true;
	});

	for (int32 i = 0; i < CachedStringTableEntries.Num(); ++i)
	{
		CachedStringTableEntries[i]->RowNumber = i + 1;
	}
}

void SAssetDifferStringTableListView::HighlightRow(const FString& RowKey)
{
	for (const auto& Entry : CachedStringTableEntries)
	{
		if (Entry->Key.Equals(RowKey))
		{
			MyListView->SetItemSelection(Entry, true);
		}
		else
		{
			MyListView->SetItemSelection(Entry, false);
		}
	}
}

void SAssetDifferStringTableListView::MergeRow(const FString& RowKey)
{
	if (StringTableVisualDiff.IsValid())
	{
		StringTableVisualDiff->PerformMerge(RowKey);
	}
}

void SAssetDifferStringTableListView::SetScrollOffset(float InScrollOffset)
{
	if (MyListView.IsValid())
	{
		MyListView->SetScrollOffset(InScrollOffset);
	}
}


TSharedRef<ITableRow> SAssetDifferStringTableListView::OnGenerateStringTableEntryRow(TSharedPtr<FCachedStringTableEntry> CachedStringTableEntry, const TSharedRef<STableViewBase>& Table)
{
	return SNew(SAssetDifferStringTableEntryRow, Table, CachedStringTableEntry, SharedThis(this)).IsLocal(bIsLocal);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
