// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAssetDifferDetailTableRowBase.h"

/**
 * 
 */
class UASSETDIFFEREDITOR_API SAssetDifferDetailSingleItemRow : public SAssetDifferDetailTableRowBase
{
public:
	SLATE_BEGIN_ARGS(SAssetDifferDetailSingleItemRow) {}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<class FAssetDifferDetailTreeNode> InOwnerTreeNode);

	virtual void Refresh() override;
	
	TSharedRef<SWidget> BuildRowContent();
	
	//~ Begin SWidget Interface
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~ End SWidget Interface

	TSharedRef<SWidget> MakeRowActionsMenu();

	/**
	 * @brief 粘贴选中Property到左侧
	 */
	void OnMenuActionMerge();

	/**
	 * @brief 拷贝选中Property的原始数据
	 */
	void OnMenuActionCopy();

	FString CopyProperty();
	
	//~ Begin SAssetDifferDetailTableRowBase Interface
	virtual void OnExpanderClicked(bool bIsExpanded) override;

	virtual int32 GetIndentLevelForBackgroundColor() override;
	//~ End SAssetDifferDetailTableRowBase Interface
	
public:
	TWeakPtr<class FAssetDifferDetailTreeNode> OwnerTreeNode;

	TSharedPtr<class SAssetDifferPropertyValueWidget> ValueWidget;
	
protected:
	const FSlateBrush* GetBorderImage() const;
	
	FSlateColor GetOuterBackgroundColor() const;
	FSlateColor GetInnerBackgroundColor() const;
	bool IsHighlighted() const;
};
