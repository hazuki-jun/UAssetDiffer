// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDetailView) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

public:
	virtual class FAssetDifferDetailColumnSizeData& GetColumnSizeData() = 0;

	virtual UDataTable* GetDataTable() { return nullptr; }

	virtual FName GetCurrentRowName() { return NAME_None; }

	virtual class SDataTableVisualDiff* GetDataTableVisualDiff() { return nullptr; }

	virtual bool IsLocalAsset() { return true; }

	void AppendCacheNodes(TArray<TSharedPtr<class FAssetDifferDetailTreeNode>> InNodes);
	
	void AddCacheNode(TSharedPtr<class FAssetDifferDetailTreeNode> InNode);
	
	int32 GetCachedNodeNum() const { return AllNodes.Num(); }
	
	const TArray<TSharedPtr<class FAssetDifferDetailTreeNode>>& GetCachedNodes();
	
protected:
	TArray<TSharedPtr<class FAssetDifferDetailTreeNode>> AllNodes;
};
