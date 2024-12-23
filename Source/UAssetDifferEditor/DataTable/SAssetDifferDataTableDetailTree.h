// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDataTableDetailTree : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDataTableDetailTree) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, class SAssetDifferDetailView* DetailView_);

	TSharedRef<ITableRow> OnGenerateRowForDetailTree( TSharedPtr<class FAssetDifferDetailTreeNode> InTreeNode, const TSharedRef<STableViewBase>& OwnerTable );
	void OnGetChildrenForDetailTree( TSharedPtr<class FAssetDifferDetailTreeNode> InTreeNode, TArray< TSharedPtr<class FAssetDifferDetailTreeNode> >& OutChildren );
	void OnItemExpansionChanged(TSharedPtr<class FAssetDifferDetailTreeNode> TreeItem, bool bIsExpanded) const;
	void HandleTableViewScrolled(double InScrollOffset);
	
	TArray<TSharedPtr<class FAssetDifferDetailTreeNode>> RootTreeNodes;

	void SetStructure(TSharedPtr<class FAssetDifferfStructOnScope>& Structure);

	TMap<FName, TArray<FProperty*>> GetStructMembers(TSharedPtr<class FAssetDifferfStructOnScope>& Structure);
	
	class SAssetDifferDetailView* DetailView = nullptr;

	const uint8* GetRowData(const FProperty* InProperty);
	
	void SetItemExpansion(bool bIsExpand, TSharedPtr<class FAssetDifferDetailTreeNode> TreeItem);

	void SetVerticalScrollOffset(float ScrollOffset);

	void RefreshEachWidget(const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& InItems);
	
	void RefreshWidgetFromItem(const TSharedPtr<class FAssetDifferDetailTreeNode>& InItem);
	
protected:
	TSharedPtr<STreeView<TSharedPtr<class FAssetDifferDetailTreeNode>>> MyTreeView;
};






