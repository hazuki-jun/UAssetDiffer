// Fill out your copyright notice in the Description page of Project Settings.


#include "SStringTableVisualDiff.h"

#include "SlateOptMacros.h"
#include "SAssetDifferStringTableListView.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"
#include "UAssetDifferEditor/AssetDifferDefinitions.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"
#include "UAssetDifferEditor/Utils/FAssetDifferStringTableUtil.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SStringTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference StringTable")));
	InitSettings();

	UAssetDifferDelegate::OnStringTableRowSelected.BindRaw(this, &SStringTableVisualDiff::OnRowSelected);
	
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				MakeToolbar()
			]
			
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SSplitter)
				.PhysicalSplitterHandleSize(5.0f)
				.HitDetectionSplitterHandleSize(5.0f)
				.Orientation(EOrientation::Orient_Horizontal)
				+ SSplitter::Slot()
				[
					SAssignNew(StringTableListViewLocal, SAssetDifferStringTableListView, SharedThis(this)).IsLocal(true)
				]

				+ SSplitter::Slot()
				[
					SAssignNew(StringTableListViewRemote, SAssetDifferStringTableListView, SharedThis(this)).IsLocal(false)
				]
			]
		]
	];
}

void SStringTableVisualDiff::OnRowSelected(bool bIsLocal, FString RowKey, int32 RowNumber)
{
	SelectedRowKey = RowKey;
	SelectedRowNumber = RowNumber;
	Sync_HighlightRow(bIsLocal, RowKey);
}

void SStringTableVisualDiff::Sync_HighlightRow(bool bIsLocal, FString RowKey)
{
	if (!StringTableListViewLocal.IsValid() || !StringTableListViewRemote.IsValid())
	{
		return;
	}

	if (bIsLocal)
	{
		StringTableListViewRemote->HighlightRow(RowKey);
	}
	else
	{
		StringTableListViewLocal->HighlightRow(RowKey);
	}
}

void SStringTableVisualDiff::Sync_VerticalScrollOffset(bool bIsLocal, float ScrollOffset)
{
	if (!StringTableListViewLocal.IsValid() || !StringTableListViewRemote.IsValid())
	{
		return;
	}
	
	if (bIsLocal)
	{
		StringTableListViewRemote->SetScrollOffset(ScrollOffset);
	}
	// else
	// {
	// 	StringTableListViewLocal->SetScrollOffset(ScrollOffset);
	// }
}

void SStringTableVisualDiff::InitSettings()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultAssetDifferAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/AssetDifferAssetsEditor.AssetDifferAssetSettings");
	
	bool Checked = true;
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyNormal"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Normal) : ClearRowViewOption(EDifferRowViewOption::Normal);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyModify"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Modify) : ClearRowViewOption(EDifferRowViewOption::Modify);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyAdded"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Added) : ClearRowViewOption(EDifferRowViewOption::Added);
	
	GConfig->GetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Removed) : ClearRowViewOption(EDifferRowViewOption::Removed);
}

void SStringTableVisualDiff::ToolbarAction_HighlightNextDifference()
{
	if (!StringTableListViewLocal.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<FCachedStringTableEntry>>& Entries = StringTableListViewLocal->CachedStringTableEntries;
	
	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		if (Entries[i]->RowNumber > SelectedRowNumber && Entries[i]->RowState != EDifferRowViewOption::Normal)
		{
			OnRowSelected(true, Entries[i]->Key, Entries[i]->RowNumber);
			OnRowSelected(false, Entries[i]->Key, Entries[i]->RowNumber);
			break;
		}
	}
}

void SStringTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	if (!StringTableListViewLocal.IsValid())
	{
		return;
	}

	TArray<TSharedPtr<FCachedStringTableEntry>>& Entries = StringTableListViewLocal->CachedStringTableEntries;
	
	for (int32 i = Entries.Num() - 1; i >= 0; --i)
	{
		if (Entries[i]->RowNumber < SelectedRowNumber && Entries[i]->RowState != EDifferRowViewOption::Normal)
		{
			OnRowSelected(true, Entries[i]->Key, Entries[i]->RowNumber);
			OnRowSelected(false, Entries[i]->Key, Entries[i]->RowNumber);
			break;
		}
	}
}

bool SStringTableVisualDiff::IsDiffable()
{
	return false;
}

void SStringTableVisualDiff::ToolbarAction_Merge()
{
	PerformMerge(SelectedRowKey);
	RefreshLayout();
}

void SStringTableVisualDiff::UpdateVisibleRows()
{
	if (!StringTableListViewLocal || !StringTableListViewRemote)
	{
		return;
	}

	FString Filter;
	
	if (!ActiveFilterText.IsEmptyOrWhitespace())
	{
		Filter = ActiveFilterText.ToString();
	}
	
	StringTableListViewLocal->FilterRows(Filter);
	StringTableListViewRemote->FilterRows(Filter);
}

void SStringTableVisualDiff::RefreshLayout()
{
	if (StringTableListViewLocal.IsValid() && StringTableListViewRemote.IsValid())
	{
		UpdateVisibleRows();
	}
}

void SStringTableVisualDiff::ModifyConfig()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultAssetDifferAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/AssetDifferAssetsEditor.AssetDifferAssetSettings");
	
	bool Checked = HasRowViewOption(EDifferRowViewOption::Normal);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyNormal"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Modify);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyModify"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Added);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyAdded"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Removed);
	GConfig->SetBool(*SectionName, TEXT("StringTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	
	GConfig->Flush(false, FileName);
}

UStringTable* SStringTableVisualDiff::GetStringTable(bool bIsLocal)
{
	return bIsLocal ? Cast<UStringTable>(LocalAsset) : Cast<UStringTable>(RemoteAsset);
}

FStringTableConstRef SStringTableVisualDiff::GetStringTableRef(bool bIsLocal)
{
	auto StringTable = GetStringTable(bIsLocal);
	check(StringTable);
	return StringTable->GetStringTable();
}

EDifferRowViewOption SStringTableVisualDiff::GetRowState(bool bIsLocal, const FString& RowKey)
{
	if (bIsLocal)
	{
		for (const auto& Entry : StringTableListViewLocal->CachedStringTableEntries)
		{
			if (Entry->Key.Equals(RowKey))
			{
				return Entry->RowState;
			}
		}
	}
	else
	{
		for (const auto& Entry : StringTableListViewRemote->CachedStringTableEntries)
		{
			if (Entry->Key.Equals(RowKey))
			{
				return Entry->RowState;
			}
		}
	}
	
	return EDifferRowViewOption::Normal;
}

void SStringTableVisualDiff::GetStringTableData(bool bIsLocal, TArray<TSharedPtr<FCachedStringTableEntry>>& OutEntries)
{
	const auto StringTableRefLocal = GetStringTableRef(true);
	const auto StringTableRefRemote = GetStringTableRef(false);

	auto SetupEntries = [&OutEntries](const FStringTableConstRef& Dest, const FStringTableConstRef& Src)
	{
		Dest->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
		{
			TSharedPtr<FCachedStringTableEntry> NewStringTableEntry = MakeShared<FCachedStringTableEntry>(InKey, InSourceString);
			NewStringTableEntry->RowState = EDifferRowViewOption::Normal;
			OutEntries.Add(NewStringTableEntry);

			return true; // continue enumeration
			
		});

		for (auto& Entry : OutEntries)
		{
			auto SrcEntry = Src->FindEntry(Entry->Key);
			if (!SrcEntry)
			{
				Entry->RowState = EDifferRowViewOption::Added;
			}
			else
			{
				if (!Entry->SourceString.Equals(SrcEntry->GetSourceString()))
				{
					Entry->RowState = EDifferRowViewOption::Modify;
				}
			}
		}

		Src->EnumerateSourceStrings([&](const FString& InKey, const FString& InSourceString)
		{
			if (!Dest->FindEntry(InKey))
			{
				TSharedPtr<FCachedStringTableEntry> NewStringTableEntry = MakeShared<FCachedStringTableEntry>(InKey, FString());
				NewStringTableEntry->RowState = EDifferRowViewOption::Removed;
				OutEntries.Add(NewStringTableEntry);			
			}
					
			return true; // continue enumeration
		});
	};
	
	if (bIsLocal)
	{
		SetupEntries(StringTableRefLocal, StringTableRefRemote);
	}
	else
	{
		SetupEntries(StringTableRefRemote, StringTableRefLocal);
	}
}

void SStringTableVisualDiff::PerformMerge(const FString& RowKey)
{
	EDifferRowViewOption RowState = static_cast<EDifferRowViewOption>(GetRowState(false, RowKey));  
	if (RowState == EDifferRowViewOption::Removed)
	{
		FAssetDifferStringTableUtil::DeleteRow(GetStringTable(true), RowKey); 
	}
	else if (RowState == EDifferRowViewOption::Added)
	{
		auto StringTable = GetStringTableRef(false);
		auto Entry = StringTable->FindEntry(RowKey);
		if (Entry.IsValid())
		{
			FAssetDifferStringTableUtil::AddRow(GetStringTable(true), RowKey, Entry->GetSourceString());
		}
	}
	else if (RowState == EDifferRowViewOption::Modify)
	{
		auto StringTable = GetStringTableRef(false);
		auto Entry = StringTable->FindEntry(RowKey);
		if (Entry.IsValid())
		{
			FAssetDifferStringTableUtil::ModifyRow(GetStringTable(true), RowKey, Entry->GetSourceString());
		}
	}

	UpdateVisibleRows();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

