#pragma once

#include "AssetDifferDetailTreeNode.h"

class FAssetDifferDetailItemNode : public FAssetDifferDetailTreeNode, public TSharedFromThis<FAssetDifferDetailItemNode>
{
public:
	FAssetDifferDetailItemNode(class SAssetDifferDetailView* DetailView_)
	: DetailView(DetailView_)
	{}
	
	virtual TSharedRef<ITableRow> GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void GetChildren(TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren) override;

	virtual class SAssetDifferDetailView* GetDetailsView() const override;

	virtual FText GetDisplayNameText() override;

	virtual FString GetPropertyValueAsString() override;
	
	virtual FText GetValueText() override;
	
	virtual void GenerateChildren();

	void GenerateMapKeyValueChildren();
	
	virtual void* GetStructData(int32 ArrayIndex = 0) override;
	
	virtual FString GetParentUniqueNodeId() const override;
	
	TWeakPtr<FAssetDifferDetailTreeNode> ParentNode;
	
	class SAssetDifferDetailView* DetailView;

	bool bIsExpanded = false;
};


