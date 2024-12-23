// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAssetDifferEditor/PropertyViewWidgets/AssetDifferDetailColumnSizeData.h"
#include "UAssetDifferEditor/PropertyViewWidgets/SAssetDifferDetailView.h"



class FAssetDifferfStructOnScope
{
public:
	class UDataTable* DataTable;
	FName CurrentRowName;
	TSharedPtr<class FStructOnScope> StructureData; 
};

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDataTableRowDetailView : public SAssetDifferDetailView
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDataTableRowDetailView) {}
		SLATE_ARGUMENT(bool, IsLocal)
		SLATE_ARGUMENT(TSharedPtr<class SDataTableVisualDiff>, DataTableVisualDiff)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	TSharedRef<SWidget> BuildRowTitle();
	
	void Refresh(const FName& InRowName);

	//~ Begin SAssetDifferDetailView Interface
	virtual FAssetDifferDetailColumnSizeData& GetColumnSizeData() override { return DetailColumnSizeData; }
	virtual UDataTable* GetDataTable() override;
	virtual FName GetCurrentRowName() override;
	virtual SDataTableVisualDiff* GetDataTableVisualDiff() override;
	virtual bool IsLocalAsset() override { return bIsLocal; }
	//~ Begin SAssetDifferDetailView Interface

	const FSlateBrush* GetCloseImage() const;
	FReply CloseButton_OnClicked();
	
	void SetItemExpansion(bool bIsExpand, int32 NodeIndex);
	void SetVerticalScrollOffset(float ScrollOffset);
	void RefreshWidgetFromItem(const TSharedPtr<class FAssetDifferDetailTreeNode> InItem);
	void RefreshForEachWidget(const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& InItems);
	
protected:
	FAssetDifferDetailColumnSizeData DetailColumnSizeData;
	
	bool bIsLocal = true;

	TSharedPtr<SButton> CloseButton;
	
	TSharedPtr<class SAssetDifferDataTableDetailRowSelector> RowSelector;
	
	TSharedPtr<class SDataTableVisualDiff> DataTableVisualDiff;

	TSharedPtr<class SAssetDifferDataTableDetailTree> MyDetailTree;
	
	FName RowName;
	
	FButtonStyle CloseButtonStyle;
};


