// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDataTableDetailTree.h"

#include "ObjectEditorUtils.h"
#include "SlateOptMacros.h"

#include "SDataTableVisualDiff.h"
#include "SAssetDifferDataTableRowDetailView.h"
#include "UAssetDifferEditor/PropertyViewWidgets/SAssetDifferDetailTableRowBase.h"
#include "..\PropertyViewWidgets\AssetDifferCategoryItemNode.h"
#include "..\PropertyViewWidgets\AssetDifferDetailTreeNode.h"
#include "UAssetDifferEditor/AssetDifferSaveGame.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDataTableDetailTree::Construct(const FArguments& InArgs, class SAssetDifferDetailView* DetailView_)
{
	DetailView = DetailView_;
	
	ChildSlot
	[
		SAssignNew(MyTreeView, STreeView<TSharedPtr<FAssetDifferDetailTreeNode>>)
		//~ Begin TreeView Interface
		.TreeItemsSource(&RootTreeNodes)
		.OnGenerateRow(this, &SAssetDifferDataTableDetailTree::OnGenerateRowForDetailTree)
		.OnGetChildren(this, &SAssetDifferDataTableDetailTree::OnGetChildrenForDetailTree)
		.OnExpansionChanged(this, &SAssetDifferDataTableDetailTree::OnItemExpansionChanged)
		.OnTreeViewScrolled(this, &SAssetDifferDataTableDetailTree::HandleTableViewScrolled)
		//~ End TreeView Interface
	];
}

TSharedRef<ITableRow> SAssetDifferDataTableDetailTree::OnGenerateRowForDetailTree(TSharedPtr<FAssetDifferDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	return InTreeNode->GenerateWidgetForTableView(OwnerTable);
}

void SAssetDifferDataTableDetailTree::OnGetChildrenForDetailTree(TSharedPtr<FAssetDifferDetailTreeNode> InTreeNode, TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren)
{
	InTreeNode->GetChildren(OutChildren);
}

void SAssetDifferDataTableDetailTree::OnItemExpansionChanged(TSharedPtr<FAssetDifferDetailTreeNode> TreeItem, bool bIsExpanded) const
{
	if (const auto DataTableVisualDiff = DetailView->GetDataTableVisualDiff())
	{
		DataTableVisualDiff->SyncDetailViewAction_Expanded(DetailView->IsLocalAsset(), bIsExpanded, TreeItem->GetNodeIndex());
	}
}

void SAssetDifferDataTableDetailTree::HandleTableViewScrolled(double InScrollOffset)
{
	if (const auto DataTableVisualDiff = DetailView->GetDataTableVisualDiff())
	{
		DataTableVisualDiff->SyncDetailViewAction_VerticalScrollOffset(DetailView->IsLocalAsset(), InScrollOffset);
	}
}

void SAssetDifferDataTableDetailTree::SetStructure(TSharedPtr<FAssetDifferfStructOnScope>& Structure)
{
	if (!Structure.IsValid())
	{
		return;
	}

	TMap<FName, TArray<FProperty*>> StructMembers = GetStructMembers(Structure);
	RootTreeNodes.Empty();
	
	for (const auto& Category : StructMembers)
	{
		TSharedPtr<FAssetDifferCategoryItemNode> CategoryNode = MakeShareable(new FAssetDifferCategoryItemNode(Category.Key, DetailView));
		for (const auto Property : Category.Value)
		{
			CategoryNode->ChildPropertyArray.Add(Property);
		}
		CategoryNode->SetNodeIndex(DetailView->GetCachedNodeNum());
		DetailView->AddCacheNode(CategoryNode);
		CategoryNode->GenerateChildren();
		
		RootTreeNodes.Add(CategoryNode);
	}

	MyTreeView->RequestTreeRefresh();
}

TMap<FName, TArray<FProperty*>> SAssetDifferDataTableDetailTree::GetStructMembers(TSharedPtr<FAssetDifferfStructOnScope>& Structure)
{
	TMap<FName, TArray<FProperty*>> StructMembers;
	
	auto ScriptStruct = Structure->StructureData->GetStruct();
	if (ScriptStruct)
	{
		for (TFieldIterator<FProperty> It(ScriptStruct); It; ++It)
		{
			FProperty* Property = *It;
			if (CastField<FObjectProperty>(Property))
			{
				continue;
			}
			
			FName CategoryName =  FObjectEditorUtils::GetCategoryFName(Property);
			if (CategoryName.IsNone())
			{
				CategoryName = Structure->CurrentRowName;
			}
			auto& Found = StructMembers.FindOrAdd(CategoryName);
			Found.Add(Property);
		}
	}

	return StructMembers;
}

const uint8* SAssetDifferDataTableDetailTree::GetRowData(const FProperty* InProperty)
{
	UDataTable* DataTable = Cast<UDataTable>(DetailView->GetDataTable());
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

void SAssetDifferDataTableDetailTree::SetItemExpansion(bool bIsExpand, TSharedPtr<FAssetDifferDetailTreeNode> TreeItem)
{
	if (MyTreeView)
	{
		MyTreeView->SetItemExpansion(TreeItem, bIsExpand);

		if (bIsExpand)
		{
			UAssetDifferSaveGame::AddExpandedCategory(DetailView->GetCurrentRowName(), TreeItem->GetCategoryName());
		}
		else
		{
			UAssetDifferSaveGame::RemoveExpandedCategory(DetailView->GetCurrentRowName(), TreeItem->GetCategoryName());
		}
	}
}

void SAssetDifferDataTableDetailTree::SetVerticalScrollOffset(float ScrollOffset)
{
	if (MyTreeView)
	{
		MyTreeView->SetScrollOffset(ScrollOffset);
	}
}

void SAssetDifferDataTableDetailTree::RefreshEachWidget(const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& InItems)
{
	if (!MyTreeView)
	{
		return;
	}

	for (auto& Item : InItems)
	{
		RefreshWidgetFromItem(Item);
	}
}

void SAssetDifferDataTableDetailTree::RefreshWidgetFromItem(const TSharedPtr<FAssetDifferDetailTreeNode>& InItem)
{
	if (MyTreeView)
	{
		auto TableRow = MyTreeView->WidgetFromItem(InItem);
		if (TableRow)
		{
			if (SAssetDifferDetailTableRowBase* SingleItemRow = static_cast<SAssetDifferDetailTableRowBase*>(TableRow.Get()))
			{
				SingleItemRow->Refresh();
			}
		}
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
