// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferStringTableEntryRow.h"
#include "SStringTableVisualDiff.h"
#include "HAL/PlatformApplicationMisc.h"
#include "SlateOptMacros.h"
#include "SAssetDifferStringTableListView.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"


namespace NS_AssetDifferStringTableEntryRow
{
	const FName StringTableDummyColumnId("Dummy");
	const FName StringTableKeyColumnId("Key");
	const FName StringTableSourceStringColumnId("SourceString");
}

#define LOCTEXT_NAMESPACE "SAssetDifferStringTableEntryRow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferStringTableEntryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, TSharedPtr<FCachedStringTableEntry> InCachedStringTableEntry, TSharedPtr<class SAssetDifferStringTableListView> InStringTableListView)
{

	bIsLocal = InArgs._IsLocal;
	CachedStringTableEntry = InCachedStringTableEntry;
	StringTableListView = InStringTableListView;
	
	FSuperRowType::Construct(FSuperRowType::
		FArguments()
		.Style(FAppStyle::Get(), "DataTableEditor.CellListViewRow"),
		OwnerTableView);
}

TSharedRef<SWidget> SAssetDifferStringTableEntryRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	TSharedPtr<SWidget> Return;
	
	FSlateColor TextColor = FLinearColor(0.9f, 0.9f, 0.9f);
	if (CachedStringTableEntry->RowState == EDifferRowViewOption::Modify)
	{
		TextColor = FLinearColor(1.0, 1.0, 0.1, 1.0);
	}
	else if (CachedStringTableEntry->RowState == EDifferRowViewOption::Added)
	{
		TextColor = FLinearColor(0.0, 0.8, 0.1, 1.0);
	}
	else if (CachedStringTableEntry->RowState == EDifferRowViewOption::Removed)
	{
		TextColor = FLinearColor(0.8, 0.0, 0.1, 1.0);
	}
	
	if (ColumnName.IsEqual(NS_AssetDifferStringTableEntryRow::StringTableKeyColumnId))
	{
		Return = SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(CachedStringTableEntry.Get()->Key))
				.ColorAndOpacity(TextColor)
#if ENGINE_MAJOR_VERSION == 4
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
#endif
			];
	}
	else if (ColumnName.IsEqual(NS_AssetDifferStringTableEntryRow::StringTableSourceStringColumnId))
	{
		Return = SNew(SOverlay)
			+ SOverlay::Slot()
			.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(FText::FromString(CachedStringTableEntry.Get()->SourceString))
				.ColorAndOpacity(TextColor)
#if ENGINE_MAJOR_VERSION == 4
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
#endif
			];
	}
	
	return Return.IsValid() ? Return.ToSharedRef() : SNullWidget::NullWidget;
}

FReply SAssetDifferStringTableEntryRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UAssetDifferDelegate::OnStringTableRowSelected.Execute(bIsLocal, CachedStringTableEntry->Key, CachedStringTableEntry->RowNumber);
	}
	
	return FReply::Handled(); 
}

FReply SAssetDifferStringTableEntryRow::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		UAssetDifferDelegate::OnStringTableRowSelected.Execute(bIsLocal, CachedStringTableEntry->Key, CachedStringTableEntry->RowNumber);	

		TSharedRef<SWidget> MenuWidget = MakeRowActionsMenu();

		FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuWidget, MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect::ContextMenu);
	}
	
	return STableRow::OnMouseButtonUp(MyGeometry, MouseEvent);
}

TSharedRef<SWidget> SAssetDifferStringTableEntryRow::MakeRowActionsMenu()
{
	FMenuBuilder MenuBuilder(true, NULL);
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_CopyName", "Copy Name"),
		LOCTEXT("DataTableRowMenuActions_CopyNamTooltip", "Copy row name"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferStringTableEntryRow::OnMenuActionCopyName))
	);
	
	MenuBuilder.AddMenuEntry(
		LOCTEXT("DataTableRowMenuActions_Copy", "Copy Value"),
		LOCTEXT("DataTableRowMenuActions_CopyTooltip", "Copy this row"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GenericCommands.Copy"),
		FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferStringTableEntryRow::OnMenuActionCopyValue))
	);

	if (!bIsLocal)
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DataTableRowMenuActions_MergeRow", "Use This Change")
			,LOCTEXT("DataTableRowMenuActions_MergeRowTooltip", "Use This Change")
			, FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.UseSelectionFromContentBrowser")
			, FUIAction(FExecuteAction::CreateRaw(this, &SAssetDifferStringTableEntryRow::OnMenuActionMerge))
		);
	}
	
	return MenuBuilder.MakeWidget();
}

void SAssetDifferStringTableEntryRow::OnMenuActionCopyName()
{
	if (CachedStringTableEntry->RowState == EDifferRowViewOption::Removed)
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*CachedStringTableEntry->Key);
}

void SAssetDifferStringTableEntryRow::OnMenuActionCopyValue()
{
	if (CachedStringTableEntry->RowState == EDifferRowViewOption::Removed)
	{
		FPlatformApplicationMisc::ClipboardCopy(TEXT("Unable to copy, maybe this row has been removed"));
		return;
	}

	FPlatformApplicationMisc::ClipboardCopy(*CachedStringTableEntry->SourceString);
}

void SAssetDifferStringTableEntryRow::OnMenuActionMerge()
{
	if (StringTableListView.IsValid())
	{
		StringTableListView->MergeRow(CachedStringTableEntry->Key);
	}
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE

