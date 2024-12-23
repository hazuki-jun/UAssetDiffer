// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDataTableListViewRow.h"

#include "IDocumentation.h"
#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"
#include "SAssetDifferDataTableLayout.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


#define LOCTEXT_NAMESPACE "SAssetDifferDataTableListViewRow"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDataTableListViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisual = InArgs._DataTableVisual;
	DataTableLayout = InArgs._DataTableLayout;
	RowDataPtr = InArgs._InRowDataPtr;

	check(RowDataPtr)
	
	SMultiColumnTableRow<FAssetDifferDataTableRowListViewDataPtr>::Construct(
		FSuperRowType::FArguments()
		.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow")
		,InOwnerTableView
	);

	FText Tooltip;
	
	if (RowDataPtr->RowState == EDifferRowViewOption::Removed)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Removed row {0}"), { *RowDataPtr->RowId.ToString() }));
	}
	else if (RowDataPtr->RowState == EDifferRowViewOption::Added)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Added row {0}"), { *RowDataPtr->RowId.ToString() }));
	}
	else if (RowDataPtr->RowState == EDifferRowViewOption::Modify)
	{
		Tooltip =  FText::FromString(FString::Format(TEXT("Row {0} changed"), { *RowDataPtr->RowId.ToString() }));
	}
	else
	{
		// SetEnabled(false);
		SetColorAndOpacity(FLinearColor(0.2, 0.2, 0.2, 1.0));
	}
	
	SetToolTipText(Tooltip);
	SetBorderImage(TAttribute<const FSlateBrush*>(this, &SAssetDifferDataTableListViewRow::GetBorder));
}

const FSlateBrush* SAssetDifferDataTableListViewRow::GetBorder() const
{
	TSharedRef< ITypedTableView<FAssetDifferDataTableRowListViewDataPtr> > OwnerTable = OwnerTablePtr.Pin().ToSharedRef();

	const bool bIsActive = OwnerTable->AsWidget()->HasKeyboardFocus();

	const bool bItemHasChildren = OwnerTable->Private_DoesItemHaveChildren( IndexInList );

	if (const FAssetDifferDataTableRowListViewDataPtr* MyItemPtr = OwnerTable->Private_ItemFromWidget(this))
	{
		const bool bIsSelected = OwnerTable->Private_IsItemSelected(*MyItemPtr);
		const bool bIsHighlighted = OwnerTable->Private_IsItemHighlighted(*MyItemPtr);

		const bool bAllowSelection = GetSelectionMode() != ESelectionMode::None;
		const bool bEvenEntryIndex = (IndexInList % 2 == 0);

		if (bIsSelected && bShowSelection)
		{
			if (bIsActive)
			{
				return IsHovered()
					? &Style->ActiveHoveredBrush
					: &Style->ActiveBrush;
			}
			else
			{
				return IsHovered()
					? &Style->InactiveHoveredBrush
					: &Style->InactiveBrush;
			}
		}
		else if (!bIsSelected && bIsHighlighted)
		{
			if (bIsActive)
			{
				return IsHovered()
					? (bEvenEntryIndex ? &Style->EvenRowBackgroundHoveredBrush : &Style->OddRowBackgroundHoveredBrush)
					: &Style->ActiveHighlightedBrush;
			}
			else
			{
				return IsHovered()
					? (bEvenEntryIndex ? &Style->EvenRowBackgroundHoveredBrush : &Style->OddRowBackgroundHoveredBrush)
					: &Style->InactiveHighlightedBrush;
			}
		}
#if ENGINE_MAJOR_VERSION == 5
		else if (bItemHasChildren && Style->bUseParentRowBrush && GetIndentLevel() == 0)
		{
			return IsHovered() 
			? &Style->ParentRowBackgroundHoveredBrush	
			: &Style->ParentRowBackgroundBrush;	
		}
#endif
		else
		{
			// Add a slightly lighter background for even rows
			if (bEvenEntryIndex)
			{
				return (IsHovered() && bAllowSelection)
					? &Style->EvenRowBackgroundHoveredBrush
					: &Style->EvenRowBackgroundBrush;

			}
			else
			{
				return (IsHovered() && bAllowSelection)
					? &Style->OddRowBackgroundHoveredBrush
					: &Style->OddRowBackgroundBrush;

			}
		}
	}

	return &Style->InactiveHoveredBrush;
}

FReply SAssetDifferDataTableListViewRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UAssetDifferDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId, RowDataPtr->RowNum);	
	}
	
	return FReply::Handled();
}

FReply SAssetDifferDataTableListViewRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);

	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UAssetDifferDelegate::OnDataTableRowSelected.Execute(bIsLocal, RowDataPtr->RowId, RowDataPtr->RowNum);	

		TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();

		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget> SAssetDifferDataTableListViewRow::MakeRowActionsMenu()
{
	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_CopyName", "Copy Name"),
		LOCTEXT("DataTableRowMenuActions_CopyNamTooltip", "Copy row name"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDataTableListViewRow::OnMenuActionCopyName))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_Copy", "Copy Value"),
		LOCTEXT("DataTableRowMenuActions_CopyTooltip", "Copy this row"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), "GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDataTableListViewRow::OnMenuActionCopyValue))
	);
	
	if (!bIsLocal)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DataTableRowMenuActions_MergeRow", "Use This Change"),
			LOCTEXT("DataTableRowMenuActions_MergeRowTooltip", "Use This Change"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.UseSelectionFromContentBrowser"),
			FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDataTableListViewRow::OnMenuActionMerge))
		);
	}

	if (RowDataPtr->RowState == EDifferRowViewOption::Modify)
	{
		MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_ShowDifference", "Show Difference"),
		LOCTEXT("DataTableRowMenuActions_ShowDifferencTooltip", "Show Difference"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
			FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferDataTableListViewRow::OnMenuActionShowDifference))
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void SAssetDifferDataTableListViewRow::OnMenuActionCopyName()
{
	if (DataTableVisual)
	{
		DataTableVisual->CopyRowName(RowDataPtr->RowId);
	}
}

void SAssetDifferDataTableListViewRow::OnMenuActionCopyValue()
{
	if (!RowDataPtr.IsValid())
	{
		return;
	}

	if (DataTableVisual)
	{
		if (RowDataPtr->RowState != EDifferRowViewOption::Removed)
		{
			DataTableVisual->CopyRow(bIsLocal, RowDataPtr->RowId);
		}
		else
		{
			DataTableVisual->CopyRow(bIsLocal, NAME_None);
		}
	}
}

void SAssetDifferDataTableListViewRow::OnMenuActionShowDifference()
{
	if (DataTableVisual)
	{
		DataTableVisual->ShowDifference_RowToRow(RowDataPtr->RowId, RowDataPtr->RowNum);
	}
}

void SAssetDifferDataTableListViewRow::OnMenuActionMerge()
{
	if (DataTableVisual)
	{
		if (RowDataPtr->RowState == EDifferRowViewOption::Removed)
		{
			DataTableVisual->MergeAction_DeleteRow(RowDataPtr->RowId);	
		}
		else
		{
			DataTableVisual->MergeAction_MergeRow(RowDataPtr->RowId);
		}
	}
}

FReply SAssetDifferDataTableListViewRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (RowDataPtr->RowState == EDifferRowViewOption::Modify)
		{
			OnMenuActionShowDifference();
		}
	}
	
	return STableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SAssetDifferDataTableListViewRow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	STableRow::OnKeyDown(MyGeometry, InKeyEvent);
	return FReply::Handled();
}

TSharedRef<SWidget> SAssetDifferDataTableListViewRow::GenerateWidgetForColumn(const FName& InColumnId)
{
	ColumnId = InColumnId;
	
	if (InColumnId.IsEqual(TEXT("RowNumber")))
	{
		return SNew(SBox)
			.Padding(FMargin(4, 2, 4, 2))
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			[
				SNew(STextBlock)
#if ENGINE_MAJOR_VERSION == 4
				.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
				.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
				.Text(FText::FromString(FString::FromInt(RowDataPtr->RowNum)))
				.ColorAndOpacity(this, &SAssetDifferDataTableListViewRow::HandleTextColorAndOpacity)
			];
	}
	
	if (InColumnId.IsEqual(TEXT("RowName")))
	{
		return SNew(SBox)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Padding(FMargin(4, 2, 4, 2))
			[
				SNew(SInlineEditableTextBlock)
				.IsReadOnly(true)
				.Text(FText::FromName(RowDataPtr->RowId))
				.ColorAndOpacity(this, &SAssetDifferDataTableListViewRow::HandleTextColorAndOpacity)
			];
	}
	
	return MakeCellWidget(InColumnId);
}

FSlateColor SAssetDifferDataTableListViewRow::HandleTextColorAndOpacity() const
{
	return DataTableLayout->GetCellTextColor(ColumnId, RowDataPtr->RowId);
}

TSharedRef<SWidget> SAssetDifferDataTableListViewRow::MakeCellWidget(const FName& InColumnId)
{
	return SNew(SBox)
		.Padding(FMargin(4, 2, 4, 2))
		[
			SNew(STextBlock)
			.IsEnabled(DataTableLayout->IsCellEnable(InColumnId, RowDataPtr->RowId))
			.Margin(FMargin(5.f, 0.f, 0.f, 0.f))
#if ENGINE_MAJOR_VERSION == 4
			.TextStyle(FEditorStyle::Get(), "DataTableEditor.CellText")
#else
			.TextStyle(FAppStyle::Get(), "DataTableEditor.CellText")
#endif
			.Text(DataTableLayout->GetCellText(InColumnId, RowDataPtr->RowId))
			.ColorAndOpacity(this, &SAssetDifferDataTableListViewRow::HandleTextColorAndOpacity)
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
