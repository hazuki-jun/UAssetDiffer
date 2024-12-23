#include "AssetDifferCategoryItemNode.h"

#include "SAssetDifferDetailCategoryRow.h"
#include "SAssetDifferDetailView.h"
#include "AssetDifferDetailItemNode.h"


TSharedRef<ITableRow> FAssetDifferCategoryItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SAssetDifferDetailCategoryRow, OwnerTable, AsShared()).CategoryName(CategoryName);
}

void FAssetDifferCategoryItemNode::GetChildren(TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren)
{
	for (auto Child : Children)
	{
		OutChildren.Add(Child);
	}
}

SAssetDifferDetailView* FAssetDifferCategoryItemNode::GetDetailsView() const
{
	return DetailView;
}

void FAssetDifferCategoryItemNode::GenerateChildren()
{
	for (const auto ChildProperty : ChildPropertyArray)
	{
		TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = MakeShareable(new FAssetDifferDetailItemNode(DetailView));
		{
			DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());
			DetailView->AddCacheNode(DetailItemNode);
			DetailItemNode->ParentNode = AsShared();
			DetailItemNode->Property = ChildProperty;
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName());
			if (CastField<FArrayProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else if (CastField<FSetProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else if (CastField<FMapProperty>(ChildProperty.Get()))
			{
				DetailItemNode->ValueText = DataTableUtils::GetPropertyValueAsText(ChildProperty.Get(), (uint8*)GetStructData(0));
			}
			else
			{
				DetailItemNode->ValueText = GetValueTextStructInternal(ChildProperty.Get());
			}
			DetailItemNode->DisplayNameText = ChildProperty->GetDisplayNameText();
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
}

void* FAssetDifferCategoryItemNode::GetStructData(int32 ArrayIndex)
{
	if (!DetailView)
	{
		return nullptr;
	}

	if (auto DataTable = DetailView->GetDataTable())
	{
		for (const auto& RowIt : DataTable->GetRowMap())
		{
			if (RowIt.Key.IsEqual(DetailView->GetCurrentRowName()))
			{
				return RowIt.Value;
			}
		}
	}

	return nullptr;
}

FName FAssetDifferCategoryItemNode::GetCategoryName()
{
	return CategoryName;
}

FString FAssetDifferCategoryItemNode::GetParentUniqueNodeId() const
{
	if (CategoryName.IsNone())
	{
		return FString();
	}
	
	return CategoryName.ToString();
}
