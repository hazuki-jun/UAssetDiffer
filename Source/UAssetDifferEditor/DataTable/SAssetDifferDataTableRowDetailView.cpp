// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDataTableRowDetailView.h"
#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "SAssetDifferDataTableDetailRowSelector.h"
#include "SAssetDifferDataTableDetailTree.h"
#include "..\PropertyViewWidgets\AssetDifferDetailTreeNode.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDataTableRowDetailView::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisualDiff = InArgs._DataTableVisualDiff;
	check(DataTableVisualDiff)
	
	FWindowStyle WindowStyle = FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window");
	CloseButtonStyle = WindowStyle.CloseButtonStyle;
	
	SAssignNew(MyDetailTree, SAssetDifferDataTableDetailTree, this);
	this->ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				BuildRowTitle()
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				[
					SAssignNew(CloseButton, SButton)
					.IsFocusable(false)
					.ContentPadding(0.0f)
					.OnClicked(this, &SAssetDifferDataTableRowDetailView::CloseButton_OnClicked)
					.Cursor(EMouseCursor::Default)
					.ToolTipText(FText::FromString(TEXT("Close detail view")))
					.ButtonStyle(FCoreStyle::Get(), "NoBorder")
					[
						SNew(SImage).Image(this, &SAssetDifferDataTableRowDetailView::GetCloseImage)
					]
				]
			]
		]

		+ SVerticalBox::Slot()
		[
			MyDetailTree.ToSharedRef()
		]
	];
}

const FSlateBrush* SAssetDifferDataTableRowDetailView::GetCloseImage() const
{
	if (CloseButton->IsPressed())
	{
		return &CloseButtonStyle.Pressed;
	}

	if (CloseButton->IsHovered())
	{
		return &CloseButtonStyle.Hovered;
	}

	return &CloseButtonStyle.Normal;
}

FReply SAssetDifferDataTableRowDetailView::CloseButton_OnClicked()
{
	if (DataTableVisualDiff)
	{
		DataTableVisualDiff->CloseDetailView();
		
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

TSharedRef<SWidget> SAssetDifferDataTableRowDetailView::BuildRowTitle()
{
	TArray<FName> Rows;
	if (DataTableVisualDiff)
	{
		if (auto DataTable = Cast<UDataTable>(DataTableVisualDiff->GetAssetObject(bIsLocal)))
		{
			Rows = DataTable->GetRowNames();
		}
	}
	
	return SAssignNew(RowSelector, SAssetDifferDataTableDetailRowSelector).Rows(Rows).SelectedRowName(RowName);
}

void SAssetDifferDataTableRowDetailView::Refresh(const FName& InRowName)
{
	AllNodes.Empty();
	
	if (!MyDetailTree.IsValid())
	{
		return;
	}

	if (DataTableVisualDiff)
	{
		RowName = InRowName;
		if (RowSelector.IsValid())
		{
			RowSelector->OnRowSelected(RowName);
		}
		
		TSharedPtr<FAssetDifferfStructOnScope> StructOnScope = MakeShareable(new FAssetDifferfStructOnScope());
		StructOnScope->DataTable = Cast<UDataTable>(DataTableVisualDiff->GetLocalAsset());
		StructOnScope->CurrentRowName = InRowName;
		StructOnScope->StructureData = DataTableVisualDiff->GetStructure();
		
		MyDetailTree->SetStructure(StructOnScope);
		SetVisibility(EVisibility::SelfHitTestInvisible);
	}
}

UDataTable* SAssetDifferDataTableRowDetailView::GetDataTable()
{
	if (DataTableVisualDiff)
	{
		return Cast<UDataTable>(DataTableVisualDiff->GetAssetObject(bIsLocal));
	}
	
	return SAssetDifferDetailView::GetDataTable();
}

FName SAssetDifferDataTableRowDetailView::GetCurrentRowName()
{
	return RowName;
}

SDataTableVisualDiff* SAssetDifferDataTableRowDetailView::GetDataTableVisualDiff()
{
	return DataTableVisualDiff.Get();
}

void SAssetDifferDataTableRowDetailView::SetItemExpansion(bool bIsExpand, int32 NodeIndex)
{
	if (!MyDetailTree)
	{
		return;
	}

	for (int32 i = 0; i < AllNodes.Num(); ++i)
	{
		if (AllNodes[i]->GetNodeIndex() == NodeIndex)
		{
			if (AllNodes[i]->bIsExpanded != bIsExpand)
			{
				AllNodes[i]->bIsExpanded =  bIsExpand;
				MyDetailTree->SetItemExpansion(bIsExpand, AllNodes[i]);
			}
			return;
		}
	}
}

void SAssetDifferDataTableRowDetailView::SetVerticalScrollOffset(float ScrollOffset)
{
	if (MyDetailTree)
	{
		MyDetailTree->SetVerticalScrollOffset(ScrollOffset);
	}
}

void SAssetDifferDataTableRowDetailView::RefreshWidgetFromItem(const TSharedPtr<FAssetDifferDetailTreeNode> InItem)
{
	if (MyDetailTree)
	{
		MyDetailTree->RefreshWidgetFromItem(InItem);
	}
}

void SAssetDifferDataTableRowDetailView::RefreshForEachWidget(const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& InItems)
{
	if (MyDetailTree)
	{
		MyDetailTree->RefreshEachWidget(InItems);
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
