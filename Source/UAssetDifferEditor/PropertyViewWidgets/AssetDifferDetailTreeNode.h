﻿#pragma once

#include "CoreMinimal.h"

class FAssetDifferDetailTreeNode
{
public:
	FAssetDifferDetailTreeNode();
	virtual ~FAssetDifferDetailTreeNode();

	/**
	 * Generates the widget representing this node
	 *
	 * @param OwnerTable		The table owner of the widget being generated
	 */
	virtual TSharedRef< ITableRow > GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable) = 0;

	/**
	 * Gets child tree nodes
	 *
	 * @param OutChildren [out]	The array to add children to
	 */
	virtual void GetChildren(TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren) = 0;

	/** @return The details view that this node is in */
	virtual class SAssetDifferDetailView* GetDetailsView() const = 0;
	
	virtual void* GetStructData(int32 ArrayIndex = 0) { return nullptr; }

	bool IsMapElement() const;
	
	bool IsContainerNode() const;

	bool IsInContainer() const { return ContainerIndex >= 0; }

	bool IsMapKeyOrValue();
	
	TWeakFieldPtr<const class FProperty> Property;
	
	uint8* RawPtr = nullptr;
	
	int32 ContainerIndex = -1;

	int32 GetNodeIndex() const { return NodeIndex; }

	virtual FString GetPropertyValueAsString();
	
	virtual FText GetValueText();

	virtual FText GetDisplayNameText();
	
	void SetNodeIndex(int32 InNodeIndex) { NodeIndex = InNodeIndex; }
	
	FString GetUniqueNodeId() const;

	virtual FString GetParentUniqueNodeId() const;
	
	void SetNodeId(const FString& InUniqueNodeId) { NodeId = InUniqueNodeId; }
	
	virtual FName GetCategoryName();
	
	bool HasAnyDifferenceRecurse();
	
	const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& GetChildNodes();

protected:
	virtual FText GetValueTextStructInternal(const FProperty* InProperty);
	virtual FText GetValueTextContainerInternal(void* PropertyData, const FProperty* InProperty);
	virtual FText GetValueTextInternal(void* ValueAddress, const FProperty* InProperty);

public:
	FText DisplayNameText;
	
	bool bHasAnyDifference = false;
	
	FText ValueText;
	
	bool bIsExpanded = false;

	bool bIsMapElement = false;
protected:
	TArray<TSharedPtr<FAssetDifferDetailTreeNode>> Children;

	int32 NodeIndex = -1;

	FString NodeId;
};
