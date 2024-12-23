#pragma once

#include "AssetDifferDetailTreeNode.h"

class FAssetDifferCategoryItemNode : public FAssetDifferDetailTreeNode, public TSharedFromThis<FAssetDifferCategoryItemNode>
{
public:
	FAssetDifferCategoryItemNode(FName CategoryName_, class SAssetDifferDetailView* DetailView_)
	: CategoryName(CategoryName_)
	, DetailView(DetailView_)
	{}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren) override;

	virtual class SAssetDifferDetailView* GetDetailsView() const override;

	virtual void GenerateChildren();

	virtual void* GetStructData(int32 ArrayIndex = 0) override;

	virtual FName GetCategoryName() override;

	virtual FString GetParentUniqueNodeId() const override;
	
	FName CategoryName;

	class SAssetDifferDetailView* DetailView;
	
	TArray<TWeakFieldPtr<const class FProperty>> ChildPropertyArray;
};
