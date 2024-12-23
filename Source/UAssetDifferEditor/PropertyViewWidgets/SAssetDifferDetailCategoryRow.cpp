// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDetailCategoryRow.h"

#include "SlateOptMacros.h"
#include "AssetDifferDetailTreeNode.h"
#include "SAssetDifferDetailExpanderArrow.h"
#include "SAssetDifferDetailView.h"
#include "UAssetDifferEditor/DataTable/SDataTableVisualDiff.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDetailCategoryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TWeakPtr<class FAssetDifferDetailTreeNode> OwnerTreeNode_)
{
	OwnerTreeNode = OwnerTreeNode_;
	const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& ChildNodes = OwnerTreeNode.Pin()->GetChildNodes();
	bool bHasAnyDifference = false;
	bHasAnyDifference = OwnerTreeNode.Pin()->HasAnyDifferenceRecurse();
	// for (int32 i = 0; i < ChildNodes.Num(); ++i)
	// {
	// 	if (ChildNodes[i] && ChildNodes[i]->bHasAnyDifference)
	// 	{
	// 		OwnerTreeNode.Pin()->bHasAnyDifference = true;
	// 		bHasAnyDifference = true;
	// 		break;
	// 	}
	// }
	
	if (bHasAnyDifference)
	{
		DisplayNameColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else
	{
		DisplayNameColor = FLinearColor(1.f, 1.f, 1.f, 1.f);	
	}
	
	TSharedRef<SHorizontalBox> HeaderBox = SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(0, 0, 0, 0)
		.AutoWidth()
		[
			SNew(SAssetDifferDetailExpanderArrow, SharedThis(this))
			.OnExpanderClicked(this, &SAssetDifferDetailCategoryRow::OnExpanderClicked)
		]
		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.Padding(4, 0, 0, 0)
		.FillWidth(1)
		[
			SNew(STextBlock)
			.Text(FText::FromName(InArgs._CategoryName))
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle")
			.ColorAndOpacity(this, &SAssetDifferDetailCategoryRow::GetDisplayNameColor)
		];
	
	this->ChildSlot
	[
		SNew(SBorder)
		.BorderImage(this, &SAssetDifferDetailCategoryRow::GetBackgroundImage)
		.Padding(FMargin(0, 0, 0, 0))
		[
			SNew(SBox)
			.MinDesiredHeight(26.f)
			[
				HeaderBox
			]
		]
	];

	STableRow< TSharedPtr< FAssetDifferDetailTreeNode > >::ConstructInternal(
	STableRow::FArguments()
		.Style( FAppStyle::Get(), "DetailsView.TreeView.TableRow" )
		.ShowSelection(false),
		InOwnerTableView);
}

void SAssetDifferDetailCategoryRow::OnExpanderClicked(bool bIsExpanded)
{
	if (!OwnerTreeNode.IsValid())
	{
		return;
	}

	auto DetailView = OwnerTreeNode.Pin()->GetDetailsView();
	if (!DetailView)
	{
		return;
	}

	auto DataTableVisual = DetailView->GetDataTableVisualDiff();
	
	if (!DataTableVisual)
	{
		return;
	}
	
	DataTableVisual->SyncDetailViewAction_Expanded(DetailView->IsLocalAsset(), bIsExpanded, OwnerTreeNode.Pin()->GetNodeIndex());
}

void SAssetDifferDetailCategoryRow::Refresh()
{
	bool bHasAnyDifference = false;
	bHasAnyDifference = OwnerTreeNode.Pin()->HasAnyDifferenceRecurse();
	// for (int32 i = 0; i < ChildNodes.Num(); ++i)
	// {
	// 	if (ChildNodes[i] && ChildNodes[i]->bHasAnyDifference)
	// 	{
	// 		OwnerTreeNode.Pin()->bHasAnyDifference = true;
	// 		bHasAnyDifference = true;
	// 		break;
	// 	}
	// }
	
	if (bHasAnyDifference)
	{
		DisplayNameColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else
	{
		DisplayNameColor = FLinearColor(1.f, 1.f, 1.f, 1.f);	
	}
}

const FSlateBrush* SAssetDifferDetailCategoryRow::GetBackgroundImage() const
{
	if (IsHovered())
	{
		return IsItemExpanded()
			? FAppStyle::GetBrush("DetailsView.CategoryTop_Hovered")
			: FAppStyle::GetBrush("DetailsView.CollapsedCategory_Hovered");
	}
	else
	{
		return IsItemExpanded()
			? FAppStyle::GetBrush("DetailsView.CategoryTop")
			: FAppStyle::GetBrush("DetailsView.CollapsedCategory");
	}
}

FSlateColor SAssetDifferDetailCategoryRow::GetInnerBackgroundColor() const
{
	return FSlateColor(FLinearColor::White);
}

FSlateColor SAssetDifferDetailCategoryRow::GetOuterBackgroundColor() const
{
	if (IsHovered())
	{
		return FAppStyle::Get().GetSlateColor("Colors.Header");
	}

	return FAppStyle::Get().GetSlateColor("Colors.Panel");
}

FSlateColor SAssetDifferDetailCategoryRow::GetDisplayNameColor() const
{
	return DisplayNameColor;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
