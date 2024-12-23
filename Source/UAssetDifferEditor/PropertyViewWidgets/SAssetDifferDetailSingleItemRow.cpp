// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDetailSingleItemRow.h"

#include "SlateOptMacros.h"

#include "HAL/PlatformApplicationMisc.h"
#include "AssetDifferDetailTreeNode.h"
#include "SAssetDifferDetailExpanderArrow.h"
#include "SAssetDifferDetailRowIndent.h"
#include "SAssetDifferDetailView.h"
#include "SAssetDifferPropertyNameWidget.h"
#include "SAssetDifferPropertyValueWidget.h"
#include "AssetDifferDetailColumnSizeData.h"
#include "UAssetDifferEditor/DataTable/SDataTableVisualDiff.h"
#include "UAssetDifferEditor/Utils/FAssetDifferDataTableUtil.h"

#define LOCTEXT_NAMESPACE "SAssetDifferDetailSingleItemRow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDetailSingleItemRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, TSharedRef<FAssetDifferDetailTreeNode> InOwnerTreeNode)
{
	OwnerTreeNode = InOwnerTreeNode;
	check(OwnerTreeNode.Pin());
	
	this->ChildSlot
	[
		BuildRowContent()
	];

	STableRow< TSharedPtr< FAssetDifferDetailTreeNode > >::ConstructInternal(
		STableRow::FArguments()
			.Style(FAppStyle::Get(), "DetailsView.TreeView.TableRow")
			.ShowSelection(false),
			InOwnerTableView);
}

void SAssetDifferDetailSingleItemRow::Refresh()
{
	auto AttachedWidget = this->ChildSlot.DetachWidget();
	if (AttachedWidget)
	{
		AttachedWidget.Reset();
	}
	this->ChildSlot.AttachWidget(BuildRowContent());
}

TSharedRef<SWidget> SAssetDifferDetailSingleItemRow::BuildRowContent()
{
	SAssetDifferDetailView* DetailsView =  OwnerTreeNode.Pin()->GetDetailsView();
	FAssetDifferDetailColumnSizeData& ColumnSizeData = DetailsView->GetColumnSizeData();
	
	TSharedRef<SWidget> BroderWidget = SNew(SBorder)
	.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
		.BorderBackgroundColor(this, &SAssetDifferDetailSingleItemRow::GetInnerBackgroundColor)
		.Padding(0)
		[
			SNew(SSplitter)
			.Style(FAppStyle::Get(), "DetailsView.Splitter")
			.PhysicalSplitterHandleSize(1.0f)
			.HitDetectionSplitterHandleSize(5.0f)
			+ SSplitter::Slot()
			.Value(ColumnSizeData.GetWholeRowColumnWidth())
			.OnSlotResized(ColumnSizeData.GetOnWholeRowColumnResized())
			[
				SNew(SAssetDifferPropertyNameWidget, OwnerTreeNode)
			]

			+ SSplitter::Slot()
			.Value(ColumnSizeData.GetWholeRowColumnWidth())
			.OnSlotResized(ColumnSizeData.GetOnWholeRowColumnResized())
			[
				SAssignNew(ValueWidget, SAssetDifferPropertyValueWidget, OwnerTreeNode)
			]
		];


	return SNew( SBorder )
		.BorderImage(this, &SAssetDifferDetailSingleItemRow::GetBorderImage)
		.Padding(FMargin(0,0,0,1))
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			SNew(SBox)
			.MinDesiredHeight(22.f)
			[
				SNew( SHorizontalBox )
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew( SBorder )
					.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
					.BorderBackgroundColor(this, &SAssetDifferDetailSingleItemRow::GetOuterBackgroundColor)
					.Padding(0)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SAssetDifferDetailRowIndent, SharedThis(this))
						]
						
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle"))
							.BorderBackgroundColor(this, &SAssetDifferDetailSingleItemRow::GetInnerBackgroundColor)
							[
								SNew(SBox)
								.WidthOverride(16.f)
								.HeightOverride(16.f)
								[
									SNew(SAssetDifferDetailExpanderArrow, SharedThis(this))
									.OnExpanderClicked(this, &SAssetDifferDetailSingleItemRow::OnExpanderClicked)
								]
							]
						]
						+ SHorizontalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							BroderWidget
						]
					]
				]
			]
		];
}

FReply SAssetDifferDetailSingleItemRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
    	TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();
    	FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
    	FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
    }
    
    return STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
}

TSharedRef<SWidget> SAssetDifferDetailSingleItemRow::MakeRowActionsMenu()
{
	if (!OwnerTreeNode.IsValid() || !OwnerTreeNode.Pin()->GetDetailsView())
	{
		return SNullWidget::NullWidget;
	}

	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
				LOCTEXT("DetailSingleItemRowActions_Copy", "Copy"),
				LOCTEXT("DetailSingleItemRowActions_CopyTooltip", "Copy this property value"),
				FSlateIcon(FAppStyle::Get().GetStyleSetName(), "GenericCommands.Copy"),
				FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDetailSingleItemRow::OnMenuActionCopy)));
	
	if (!OwnerTreeNode.Pin()->GetDetailsView()->IsLocalAsset()
		&& OwnerTreeNode.Pin()->HasAnyDifferenceRecurse()
		&& !OwnerTreeNode.Pin()->IsMapElement())
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DetailSingleItemRowActions_MergeRow", "Use This Change"),
			LOCTEXT("DetailSingleItemRowActions_MergeRowTooltip", "Use This Change"),
			FSlateIcon(FAppStyle::Get().GetStyleSetName(), "ContentReference.UseSelectionFromContentBrowser"),
			FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDetailSingleItemRow::OnMenuActionMerge))
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void SAssetDifferDetailSingleItemRow::OnMenuActionMerge()
{
	auto DataTableVisual = OwnerTreeNode.Pin()->GetDetailsView()->GetDataTableVisualDiff();
	if (!DataTableVisual)
	{
		return;
	}

	FString PropertyString = CopyProperty();
	if (OwnerTreeNode.Pin()->IsContainerNode())
	{
		DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), PropertyString, true);
	}
	else
	{
		DataTableVisual->DetailViewAction_MergeProperty(OwnerTreeNode.Pin()->GetNodeIndex(), PropertyString, false);
		OwnerTreeNode.Pin()->bHasAnyDifference = false;
		Refresh();
	}
}

void SAssetDifferDetailSingleItemRow::OnMenuActionCopy()
{
	FString FormattedString = CopyProperty();
	FPlatformApplicationMisc::ClipboardCopy(*FormattedString);
}

FString SAssetDifferDetailSingleItemRow::CopyProperty()
{
	return FAssetDifferDataTableUtil::CopyProperty(OwnerTreeNode.Pin());
}

void SAssetDifferDetailSingleItemRow::OnExpanderClicked(bool bIsExpanded)
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

int32 SAssetDifferDetailSingleItemRow::GetIndentLevelForBackgroundColor()
{
	IndentLevel = GetIndentLevel() - 1;
	return IndentLevel;
}

const FSlateBrush* SAssetDifferDetailSingleItemRow::GetBorderImage() const
{
// #if ENGINE_MAJOR_VERSION == 4
	if( IsHighlighted() )
	{
		return FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle_Highlighted");
	}
	else if (IsHovered())
	{
		return FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle_Hovered");
	}
	else
	{
		return FAppStyle::Get().GetBrush("DetailsView.CategoryMiddle");
 	}
// #else
//	return FAppStyle::Get().GetBrush("DetailsView.GridLine");
// #endif
}

FSlateColor SAssetDifferDetailSingleItemRow::GetOuterBackgroundColor() const
{
	return FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f));
}

FSlateColor SAssetDifferDetailSingleItemRow::GetInnerBackgroundColor() const
{
#if ENGINE_MAJOR_VERSION == 4
	return FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f));
#endif
	
	FSlateColor Color;
	
	if (IsHighlighted())
	{
		Color = FAppStyle::Get().GetSlateColor("Colors.Hover");
	}
	else
	{
		int32 ColorIndex = 0;
		static const uint8 ColorOffsets[] =
		{
			0, 4, (4 + 2), (6 + 4), (10 + 6)
		};
	
		const FSlateColor BaseSlateColor = IsHovered() ? 
			FAppStyle::Get().GetSlateColor("Colors.Header") : 
			FAppStyle::Get().GetSlateColor("Colors.Panel");
	
		const FColor BaseColor = BaseSlateColor.GetSpecifiedColor().ToFColor(true);
	
		const FColor ColorWithOffset(
			BaseColor.R + ColorOffsets[ColorIndex], 
			BaseColor.G + ColorOffsets[ColorIndex], 
			BaseColor.B + ColorOffsets[ColorIndex]);
	
		return FSlateColor(FLinearColor::FromSRGBColor(ColorWithOffset));
	}
	
	return Color;
}

bool SAssetDifferDetailSingleItemRow::IsHighlighted() const
{
	// TSharedPtr<FAssetDifferDetailTreeNode> OwnerTreeNodePtr = OwnerTreeNode.Pin();
	// return OwnerTreeNodePtr.IsValid() ? OwnerTreeNodePtr->IsHighlighted() : false;
	return false;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
