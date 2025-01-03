﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SDataTableVisualDiff.h"

#include "IDocumentation.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"



#include "SAssetDifferDataTableLayout.h"
#include "SAssetDifferDataTableRowDetailView.h"

#include "Framework/Commands/GenericCommands.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"
#include "..\PropertyViewWidgets\AssetDifferDetailTreeNode.h"
#include "UAssetDifferEditor/AssetDifferSaveGame.h"
#include "UAssetDifferEditor/Utils/FAssetDifferDataTableUtil.h"
#include "Widgets/Images/SLayeredImage.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SDataTableVisualDiff"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDataTableVisualDiff::Construct(const FArguments& InArgs)
{
	ParentWindow = InArgs._ParentWindow;
	LocalAsset = InArgs._LocalAsset;
	RemoteAsset = InArgs._RemoteAsset;
	WindowSize = ParentWindow->GetClientSizeInScreen();
	ParentWindow->SetTitle(FText::FromString(TEXT("Difference DataTable")));
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}

	InitSettings();
	
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
				BuildWidgetContent()
			]
		]
	];

	UAssetDifferDelegate::OnDataTableRowSelected.BindRaw(this, &SDataTableVisualDiff::OnRowSelectionChanged);
}

void SDataTableVisualDiff::InitSettings()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultAssetDifferAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/AssetDifferAssetsEditor.AssetDifferAssetSettings");
	
	bool Checked = true;
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyNormal"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Normal) : ClearRowViewOption(EDifferRowViewOption::Normal);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyModify"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Modify) : ClearRowViewOption(EDifferRowViewOption::Modify);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyAdded"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Added) : ClearRowViewOption(EDifferRowViewOption::Added);
	
	GConfig->GetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	Checked ? SetRowViewOption(EDifferRowViewOption::Removed) : ClearRowViewOption(EDifferRowViewOption::Removed);
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildWidgetContent()
{
	TAttribute<float> RowDetailViewSplitterValue;
	RowDetailViewSplitterValue.BindRaw(this, &SDataTableVisualDiff::GetRowDetailViewSplitterValue);

	TAttribute<EVisibility> RowDetailViewVisibility;
	RowDetailViewVisibility.BindRaw(this, &SDataTableVisualDiff::OnGetRowDetailViewVisibility);
	
	return SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SImage)
		.ColorAndOpacity(FSlateColor(FLinearColor(0.0, 0.0, 0.0, 1.0)))
	]
	
	+ SOverlay::Slot()
	[
		SNew(SSplitter)
		.PhysicalSplitterHandleSize(5.0f)
		.HitDetectionSplitterHandleSize(5.0f)
		.Orientation(EOrientation::Orient_Vertical)
		+ SSplitter::Slot()
		[
			SNew(SSplitter)
			.Style(FAppStyle::Get(), "DetailsView.Splitter")
			.PhysicalSplitterHandleSize(5.0f)
			.HitDetectionSplitterHandleSize(5.0f)
			+ SSplitter::Slot()
			[
				SAssignNew(DetailViewSplitterLocal, SSplitter)
				.Orientation(EOrientation::Orient_Vertical)
				+ SSplitter::Slot()
				[
					BuildLayoutWidget(FText::FromString(FString::Format(TEXT("{0} [ 工作区资源 ]"), { *LocalAsset->GetName() })), true)
				]
			]

			+ SSplitter::Slot()
			[
				SAssignNew(DetailViewSplitterRemote, SSplitter)
				.Orientation(EOrientation::Orient_Vertical)
				+ SSplitter::Slot()
				[
					BuildLayoutWidget(FText::FromString(FString::Format(TEXT("{0} [ 对比的资源 ]"), { *RemoteAsset->GetName() })), false)
				]
			]
		]
		
		+ SSplitter::Slot()
		.Value(RowDetailViewSplitterValue)
		[
			SNew(SSplitter)
			.Visibility(RowDetailViewVisibility)
			.Style(FAppStyle::Get(), "DetailsView.Splitter")
			.PhysicalSplitterHandleSize(5.0f)
			.HitDetectionSplitterHandleSize(5.0f)
			
			+ SSplitter::Slot()
			[
				BuildRowDetailView(true)
			]
			
			+ SSplitter::Slot()
			[
				BuildRowDetailView(false)
			]
		]
	];
}

EVisibility SDataTableVisualDiff::OnGetRowDetailViewVisibility() const
{
	if (RowDetailViewLocal && RowDetailViewLocal->GetVisibility() == EVisibility::SelfHitTestInvisible)
	{
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

float SDataTableVisualDiff::GetRowDetailViewSplitterValue() const
{
	if (RowDetailViewLocal && RowDetailViewLocal->GetVisibility() == EVisibility::SelfHitTestInvisible)
	{
		return 3.f;
	}
	
	return 0.f;
}

void SDataTableVisualDiff::ModifyConfig()
{
	const FString FileName = FPaths::ProjectConfigDir() / FString(TEXT("DefaultAssetDifferAssetSettings.ini"));
	const FString SectionName = TEXT("/Script/AssetDifferAssetsEditor.AssetDifferAssetSettings");
	
	bool Checked = HasRowViewOption(EDifferRowViewOption::Normal);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyNormal"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Modify);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyModify"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Added);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyAdded"), Checked, FileName);
	
	Checked = HasRowViewOption(EDifferRowViewOption::Removed);
	GConfig->SetBool(*SectionName, TEXT("DataTableVisualDiffShowOnlyRemoved"), Checked, FileName);
	
	GConfig->Flush(false, FileName);
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildLayoutWidget(FText InTitle, bool bIsLocal)
{
	return SNew(SAssetDifferDataTableLayout).Title(InTitle).IsLocal(bIsLocal).DataTableVisual(SharedThis(this));
}

TSharedRef<SWidget> SDataTableVisualDiff::BuildRowDetailView(bool bIsLocal)
{
	if (bIsLocal)
	{
		SAssignNew(RowDetailViewLocal,SAssetDifferDataTableRowDetailView)
		.Visibility(EVisibility::Collapsed)
		.IsLocal(bIsLocal)
		.DataTableVisualDiff(SharedThis(this));
		return RowDetailViewLocal.ToSharedRef();
	}
	else
	{
		SAssignNew(RowDetailViewRemote,SAssetDifferDataTableRowDetailView)
		.Visibility(EVisibility::Collapsed)
		.IsLocal(bIsLocal)
		.DataTableVisualDiff(SharedThis(this));
		return RowDetailViewRemote.ToSharedRef();
	}
}

TSharedPtr<FStructOnScope> SDataTableVisualDiff::GetStructure()
{
	UDataTable* DataTable = Cast<UDataTable>(LocalAsset); 
	return MakeShareable(new FStructOnScope(DataTable->RowStruct));
}

void SDataTableVisualDiff::UpdateVisibleRows()
{
	if (!DataTableLayoutLocal || !DataTableLayoutRemote)
	{
		return;
	}

	FString Filter;
	
	if (!ActiveFilterText.IsEmptyOrWhitespace())
	{
		Filter = ActiveFilterText.ToString();
	}
	
	DataTableLayoutLocal->FilterRows(Filter);
	DataTableLayoutRemote->FilterRows(Filter);
}

void SDataTableVisualDiff::ToolbarAction_HighlightNextDifference()
{
	if (!DataTableLayoutLocal)
	{
		return;
	}

	CloseDetailView();
	
	auto& VisibleRows = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = 0; i < VisibleRows.Num(); ++i)
	{
		if (VisibleRows[i]->RowNum > SelectedRowNumber)
		{
			if (VisibleRows[i]->RowState > EDifferRowViewOption::Normal)
			{
				DataTableLayoutLocal->SelectRow(VisibleRows[i]->RowId);
				OnRowSelectionChanged(true, VisibleRows[i]->RowId, VisibleRows[i]->RowNum);
				break;
			}
		}
	}
}

void SDataTableVisualDiff::ToolbarAction_HighlightPrevDifference()
{
	if (!DataTableLayoutLocal)
	{
		return;
	}
	
	CloseDetailView();
	
	auto& VisibleRows = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = VisibleRows.Num() - 1; i >= 0; --i)
	{
		if (VisibleRows[i]->RowNum < SelectedRowNumber)
		{
			if (VisibleRows[i]->RowState > EDifferRowViewOption::Normal)
			{
				DataTableLayoutLocal->SelectRow(VisibleRows[i]->RowId);
				OnRowSelectionChanged(true, VisibleRows[i]->RowId, VisibleRows[i]->RowNum);
				break;
			}
		}
	}
}

void SDataTableVisualDiff::ToolbarAction_Diff()
{
	if (!ToolbarAction_CanDiff())
	{
		return;
	}
	
	ShowDifference_RowToRow(SelectedRowId, SelectedRowNumber);
}

bool SDataTableVisualDiff::ToolbarAction_CanDiff()
{
	if (!DataTableLayoutLocal)
	{
		return false;
	}
	
	auto& VisibleRowsLocal = DataTableLayoutLocal->GetVisibleRows();
	for (int32 i = VisibleRowsLocal.Num() - 1; i >= 0; --i)
	{
		if (VisibleRowsLocal[i]->RowId.IsEqual(SelectedRowId))
		{
			if (VisibleRowsLocal[i]->RowState == EDifferRowViewOption::Modify)
			{
				return true;
			}
		}
	}
	
	return false;
}

void SDataTableVisualDiff::ToolbarAction_Merge()
{
	auto& VisibleRows = DataTableLayoutRemote->GetVisibleRows();
	for (int32 i = VisibleRows.Num() - 1; i >= 0; --i)
	{
		if (VisibleRows[i]->RowId.IsEqual(SelectedRowId))
		{
			if (VisibleRows[i]->RowState == EDifferRowViewOption::Removed)
			{
				MergeAction_DeleteRow(SelectedRowId);
				ToolbarAction_HighlightNextDifference();
				break;
			}
			else
			{
				MergeAction_MergeRow(SelectedRowId);
				ToolbarAction_HighlightNextDifference();
				break;
			}
		}
	}
}

void SDataTableVisualDiff::OnRowSelectionChanged(bool bIsLocal, FName RowId, int32 RowNumber)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowId;
	SelectedRowNumber = RowNumber;

	if (bIsLocal)
	{
		if (DataTableLayoutRemote)
		{
			DataTableLayoutRemote->SelectRow(RowId);
		}
	}
	else
	{
		if (DataTableLayoutLocal)
		{
			DataTableLayoutLocal->SelectRow(RowId);
		}
	}
}

void SDataTableVisualDiff::CopyRow(bool bIsLocal, const FName& RowName)
{
	bIsLocalDataTableSelected = bIsLocal;
	SelectedRowId = RowName;

	CopySelectedRow();
}

void SDataTableVisualDiff::CopySelectedRow()
{
	if (SelectedRowId.IsNone())
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}
	
	UDataTable* TablePtr = Cast<UDataTable>(bIsLocalDataTableSelected ? LocalAsset : RemoteAsset);
	uint8* RowPtr = TablePtr ? TablePtr->GetRowMap().FindRef(SelectedRowId) : nullptr;
	
	if (!RowPtr || !TablePtr->RowStruct)
	{
		SelectedRowId = NAME_None;
		return;
	}
	
	FString ClipboardValue;
	FAssetDifferDataTableUtil::ExportRowText(ClipboardValue, TablePtr, SelectedRowId);
	FPlatformApplicationMisc::ClipboardCopy(*ClipboardValue);
}

void SDataTableVisualDiff::CopyRowName(const FName& RowName)
{
	if (SelectedRowId.IsNone())
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*RowName.ToString());
}

void SDataTableVisualDiff::MergeAction_MergeRow(const FName& RowName)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	UDataTable* DataTableRemote = CastChecked<UDataTable>(RemoteAsset);
	FAssetDifferDataTableUtil::MergeRow(DataTableLocal, DataTableRemote, RowName);

	RefreshLayout();
}

void SDataTableVisualDiff::MergeAction_DeleteRow(FName RowName)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	FAssetDifferDataTableUtil::DeleteRow(DataTableLocal, RowName);

	RefreshLayout();
}

void SDataTableVisualDiff::ShowDifference_RowToRow(const FName& RowName, int32 InSelectedRowNumber)
{
	if (!RowDetailViewLocal || !RowDetailViewRemote)
	{
		return;
	}

	SelectedRowId = RowName;
	SelectedRowNumber = InSelectedRowNumber;
	
	RowDetailViewLocal->Refresh(RowName);
	RowDetailViewRemote->Refresh(RowName);

	DiffAndSetRow();
	
	ExpandCategories();
}

void SDataTableVisualDiff::DiffAndSetRow()
{
	const auto& CacheNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& CacheNodesRemote = RowDetailViewRemote->GetCachedNodes();
	
	for (int32 i = 0; i < CacheNodesRemote.Num(); ++i)
	{
		int32 NodeIndexLocal = GetRealLocalDetailTreeNodeIndex(i);
		if (NodeIndexLocal < 0)
		{
			continue;
		}

		FString A = CacheNodesLocal[NodeIndexLocal]->GetValueText().ToString();
		FString B = CacheNodesRemote[i]->GetValueText().ToString();
		
		if (auto TextProp = CastField<FTextProperty>(CacheNodesRemote[i]->Property.Get()))
		{
			A = FAssetDifferDataTableUtil::CopyProperty(CacheNodesLocal[NodeIndexLocal]);
			B = FAssetDifferDataTableUtil::CopyProperty(CacheNodesRemote[i]);
		}

		const bool bHasAnyDifference = !A.Equals(B);
		CacheNodesLocal[NodeIndexLocal]->bHasAnyDifference = bHasAnyDifference;
		CacheNodesRemote[i]->bHasAnyDifference = bHasAnyDifference;
	}
}

void SDataTableVisualDiff::RefreshLayout()
{
	if (!DataTableLayoutLocal || !DataTableLayoutRemote)
	{
		return;
	}

	DataTableLayoutLocal->SetupRowsData();
	DataTableLayoutRemote->SetupRowsData();
	UpdateVisibleRows();
	
	if (RowDetailViewLocal && RowDetailViewRemote)
	{
		RowDetailViewLocal->SetVisibility(EVisibility::Collapsed);
		RowDetailViewRemote->SetVisibility(EVisibility::Collapsed);
	}
}

void SDataTableVisualDiff::ExpandCategories()
{
	if (!RowDetailViewLocal || !RowDetailViewRemote)
	{
		return;
	}

	auto& AllNodes = RowDetailViewLocal->GetCachedNodes();
	for (const auto& TreeNode : AllNodes)
	{
		if (TreeNode->IsContainerNode())
		{
			if (UAssetDifferSaveGame::IsRowCategoryExpanded(SelectedRowId, TreeNode->GetCategoryName()))
			{
				RowDetailViewLocal->SetItemExpansion(true, TreeNode->GetNodeIndex());
				RowDetailViewRemote->SetItemExpansion(true, TreeNode->GetNodeIndex());
			}
		}
	}
}

FReply SDataTableVisualDiff::OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::LeftControl)
	{
		bPressedCtrl = true;
	}
	
	if (InKeyEvent.GetKey() == EKeys::C)
	{
		if (bPressedCtrl)
		{
			CopySelectedRow();
			bPressedCtrl = false;
		}
	}
	
	return SCompoundWidget::OnPreviewKeyDown(MyGeometry, InKeyEvent);
}

FReply SDataTableVisualDiff::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	bPressedCtrl = false;
	return SCompoundWidget::OnKeyUp(MyGeometry, InKeyEvent);
}

const uint8* SDataTableVisualDiff::GetPropertyData(const FProperty* InProperty)
{
	UDataTable* DataTable = Cast<UDataTable>(LocalAsset);
	if (!DataTable)
	{
		return nullptr;
	}

	auto RowMap = DataTable->GetRowMap();
	for (auto RowIt = RowMap.CreateConstIterator(); RowIt; ++RowIt)
	{
		for (TFieldIterator<FProperty> It(DataTable->RowStruct); It; ++It)
		{
			if (*It == InProperty)
			{
				return RowIt.Value();
			}
		}
	}
	
	return nullptr;
}

void SDataTableVisualDiff::SyncDetailViewAction_Expanded(bool bIsLocal, bool bIsExpanded, int32 NodeIndex)
{
	if (bIsLocal)
	{
		if (RowDetailViewRemote)
		{
			int32 NodeIndexRemote = GetRealRemoteDetailTreeNodeIndex (NodeIndex);
			RowDetailViewRemote->SetItemExpansion(bIsExpanded, NodeIndexRemote);
		}
	}
	else
	{
		if (RowDetailViewLocal)
		{
			int32 NodeIndexLocal = GetRealLocalDetailTreeNodeIndex(NodeIndex);
			RowDetailViewLocal->SetItemExpansion(bIsExpanded, NodeIndexLocal);
		}
	}
}

void SDataTableVisualDiff::SyncDetailViewAction_VerticalScrollOffset(bool bIsLocal, float ScrollOffset)
{
	if (bIsLocal)
	{
		if (RowDetailViewRemote)
		{
			RowDetailViewRemote->SetVerticalScrollOffset(ScrollOffset);
		}
	}
	// else
	// {
	// 	if (RowDetailViewLocal)
	// 	{
	// 		RowDetailViewLocal->SetVerticalScrollOffset(ScrollOffset);
	// 	}
	// }
}

void SDataTableVisualDiff::DetailViewAction_MergeProperty(int32 InNodeIndexRemote, const FString& PropertyValueString, bool bRegenerate)
{
	if (!RowDetailViewLocal)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("PasteDataTableRow", "Paste Data Table Row"));
	
	auto DestDataTable = Cast<UDataTable>(GetLocalAsset());
	DestDataTable->Modify();
	FDataTableEditorUtils::BroadcastPreChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	
	const auto& AllNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& AllNodesRemote = RowDetailViewRemote->GetCachedNodes();
	
	int32 NodeIndex = GetRealLocalDetailTreeNodeIndex(InNodeIndexRemote);

	if (NodeIndex < 0)
	{
		return;
	}
	
	auto PropertyToModify = AllNodesLocal[NodeIndex]->Property.Get();
	if (AllNodesLocal[NodeIndex]->IsInContainer())
	{
		void* ValueAddr = AllNodesLocal[NodeIndex]->RawPtr;
#if ENGINE_MAJOR_VERSION == 4
		PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
		PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
	}
	else
	{
		if (AllNodesLocal[NodeIndex]->IsContainerNode())
		{
			void* ValueAddr = AllNodesLocal[NodeIndex]->GetStructData(0);
#if ENGINE_MAJOR_VERSION == 4
			PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
			PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
		}
		else
		{
			void* StructData = AllNodesLocal[NodeIndex]->GetStructData(0);
			void* ValueAddr = PropertyToModify->ContainerPtrToValuePtr<void>(StructData);
#if ENGINE_MAJOR_VERSION == 4
			PropertyToModify->ImportText(*PropertyValueString, ValueAddr, PPF_Copy, nullptr);
#else
			PropertyToModify->ImportText_Direct(*PropertyValueString, ValueAddr, nullptr, PPF_Copy);
#endif
		}
	}

	DestDataTable->HandleDataTableChanged(SelectedRowId);
	DestDataTable->MarkPackageDirty();
	FDataTableEditorUtils::BroadcastPostChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);

	if (bRegenerate)
	{
		ShowDifference_RowToRow(SelectedRowId, SelectedRowNumber);
		ExpandCategories();
	}
	else
	{
		AllNodesLocal[NodeIndex]->ValueText = AllNodesRemote[InNodeIndexRemote]->ValueText;
		DiffAndSetRow();
		RowDetailViewLocal->RefreshForEachWidget(AllNodesLocal);
		RowDetailViewRemote->RefreshForEachWidget(AllNodesRemote);
	}

	if (!FAssetDifferDataTableUtil::HasAnyDifferenceRowToRow(GetLocalDataTable(), GetRemoteDataTable(), SelectedRowId))
	{
		if (!DataTableLayoutLocal || !DataTableLayoutRemote)
		{
			return;
		}

		auto& VisibleRowLocal = DataTableLayoutLocal->GetVisibleRows();
		auto& VisibleRowRemote = DataTableLayoutRemote->GetVisibleRows();
		for (const auto& RowData : VisibleRowLocal)
		{
			if (RowData->RowId.IsEqual(SelectedRowId))
			{
				RowData->RowState = EDifferRowViewOption::Normal;
			}
		}

		for (const auto& RowData : VisibleRowRemote)
		{
			if (RowData->RowId.IsEqual(SelectedRowId))
			{
				RowData->RowState = EDifferRowViewOption::Normal;
			}
		}
	}
}

void SDataTableVisualDiff::CloseDetailView()
{
	if (RowDetailViewLocal && RowDetailViewRemote)
	{
		if (RowDetailViewLocal->GetVisibility() != EVisibility::Collapsed)
		{
			RowDetailViewLocal->SetVisibility(EVisibility::Collapsed);
			RowDetailViewRemote->SetVisibility(EVisibility::Collapsed);	
		}
	}
}

int32 SDataTableVisualDiff::GetRealLocalDetailTreeNodeIndex(int32 RemoteNodeIndex)
{
	int32 OutIndex = -1;
	const auto& AllNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& AllNodesRemote = RowDetailViewRemote->GetCachedNodes();

	if (!AllNodesRemote.IsValidIndex(RemoteNodeIndex))
	{
		return OutIndex;
	}

	FString NodeIdRemote = AllNodesRemote[RemoteNodeIndex]->GetUniqueNodeId();
	for (int32 i = 0; i < AllNodesLocal.Num(); ++i)
	{
		FString NodeIdLocal = AllNodesLocal[i]->GetUniqueNodeId();
		if (NodeIdLocal.Equals(NodeIdRemote))
		{
			OutIndex = i;
			break;
		}
	}

	return OutIndex;
}

int32 SDataTableVisualDiff::GetRealRemoteDetailTreeNodeIndex(int32 LocalNodeIndex)
{
	int32 OutIndex = -1;
	const auto& AllNodesLocal = RowDetailViewLocal->GetCachedNodes();
	const auto& AllNodesRemote = RowDetailViewRemote->GetCachedNodes();

	if (!AllNodesLocal.IsValidIndex(LocalNodeIndex))
	{
		return OutIndex;
	}

	FString NodeIdLocal = AllNodesLocal[LocalNodeIndex]->GetUniqueNodeId();
	for (int32 i = 0; i < AllNodesRemote.Num(); ++i)
	{
		FString NodeIdRemote = AllNodesRemote[i]->GetUniqueNodeId();
		if (NodeIdRemote.Equals(NodeIdLocal))
		{
			OutIndex = i;
			break;
		}
	}

	return OutIndex;
}

int32 SDataTableVisualDiff::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                                    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 Ret = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (ParentWindow.IsValid())
	{
		FVector2D ClientSize = ParentWindow->GetClientSizeInScreen();
		if (ClientSize != WindowSize)
		{
			WindowSize = ClientSize;
			UAssetDifferDelegate::OnWindowResized.Broadcast(WindowSize);
		}
	}

	return Ret;
}

void SDataTableVisualDiff::GetDataTableData(bool bIsLocal, TArray<FDataTableEditorColumnHeaderDataPtr>& OutAvailableColumns, TArray<FDataTableEditorRowListViewDataPtr>& OutAvailableRows)
{
	UDataTable* DataTableLocal = CastChecked<UDataTable>(LocalAsset);
	UDataTable* DataTableRemote = CastChecked<UDataTable>(RemoteAsset);

	const TMap<FName, uint8*>& RowMapLocal = DataTableLocal->GetRowMap();
	TArray<FName> RowNamesLocal = DataTableLocal->GetRowNames();
	TArray<FDataTableEditorRowListViewDataPtr> EmptyRowDataRemote;
	
	const TMap<FName, uint8*>& RowMapRemote = DataTableRemote->GetRowMap(); 
	TArray<FName> RowNamesRemote = DataTableRemote->GetRowNames();
	TArray<FDataTableEditorRowListViewDataPtr> EmptyRowDataLocal;
	
	if (bIsLocal)
	{
		FDataTableEditorUtils::CacheDataTableForEditing(DataTableLocal, OutAvailableColumns, OutAvailableRows);
		
		int32 Index = 0;
		for (auto RowIt = RowMapRemote.CreateConstIterator(); RowIt; ++RowIt, ++Index)
		{
			if (!RowMapLocal.Find(RowIt.Key()))
			{
				FDataTableEditorRowListViewDataPtr DataPtr = MakeShareable(new FDataTableEditorRowListViewData());
				DataPtr->RowId = RowIt->Key;
				DataPtr->RowNum = -2;
				OutAvailableRows.Add(DataPtr);
			}
		}

		for (auto& RowData : OutAvailableRows)
		{
			if (!RowMapRemote.Find(RowData->RowId))
			{
				RowData->RowNum = 0;
			}
			else if (RowData->RowNum > 0)
			{
				if (FAssetDifferDataTableUtil::HasAnyDifferenceRowToRow(DataTableLocal, DataTableRemote, RowData->RowId))
				{
					RowData->RowNum = -1;
				}
			}
		}
	}
	else
	{
		FDataTableEditorUtils::CacheDataTableForEditing(DataTableRemote, OutAvailableColumns, OutAvailableRows);
		
		int32 Index = 0;
		for (auto RowIt = RowMapLocal.CreateConstIterator(); RowIt; ++RowIt, ++Index)
		{
			if (!RowMapRemote.Find(RowIt.Key()))
			{
				FDataTableEditorRowListViewDataPtr DataPtr = MakeShareable(new FDataTableEditorRowListViewData());
				DataPtr->RowId = RowIt->Key;
				DataPtr->RowNum = -2;
				OutAvailableRows.Add(DataPtr);
			}
		}

		for (auto& RowData : OutAvailableRows)
		{
			if (!RowMapLocal.Find(RowData->RowId))
			{
				RowData->RowNum = 0;
			}
			else if (RowData->RowNum > 0)
			{
				if (FAssetDifferDataTableUtil::HasAnyDifferenceRowToRow(DataTableLocal, DataTableRemote, RowData->RowId))
				{
					RowData->RowNum = -1;
				}
			}
		}
	}
}

FSlateColor SDataTableVisualDiff::GetHeaderColumnColorAndOpacity(bool InIsLocal, int32 Index) const
{
	if (!DataTableLayoutLocal || ! DataTableLayoutRemote)
	{
		return FLinearColor(1.f, 1.f, 1.f);
	}

	auto& AvailableColumnsLocal = DataTableLayoutLocal->GetAvailableColumns();
	auto& AvailableColumnsRemote = DataTableLayoutRemote->GetAvailableColumns();

	auto ExistColumn = [Index](TArray<FDataTableEditorColumnHeaderDataPtr>& Dest, TArray<FDataTableEditorColumnHeaderDataPtr>& Src)
	{
		if (!Dest.IsValidIndex(Index))
		{
			return true;
		}

		FName Column =  Dest[Index]->ColumnId;
		for (const auto& ColumnHeaderData : Src)
		{
			if (ColumnHeaderData->ColumnId.IsEqual(Column))
			{
				return true;
			}
		}
		
		return false;	
	};

	bool bFound = false;
	if (InIsLocal)
	{
		bFound = ExistColumn(AvailableColumnsLocal, AvailableColumnsRemote);
	}
	else
	{
		bFound = ExistColumn(AvailableColumnsRemote, AvailableColumnsLocal);
	}
	
	return bFound ? FLinearColor(1.f, 1.f, 1.f) : FLinearColor(0.0, 0.8, 0.1, 1.0);
}



void SDataTableVisualDiff::SyncVerticalScrollOffset(bool bIsLocal, float NewOffset)
{
	if (bIsLocal)
	{
		if (DataTableLayoutRemote)
		{
			DataTableLayoutRemote->SetListViewScrollOffset(NewOffset);
		}
	}
	else
	{
		if (DataTableLayoutLocal)
		{
			DataTableLayoutLocal->SetListViewScrollOffset(NewOffset);
		}
	}
}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE 
