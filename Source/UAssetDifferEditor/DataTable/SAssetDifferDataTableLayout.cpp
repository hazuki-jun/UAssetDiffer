// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDataTableLayout.h"

#include "IDocumentation.h"
#include "SDataTableVisualDiff.h"
#include "SlateOptMacros.h"

#include "SAssetDifferDataTableListViewRow.h"
#include "Fonts/FontMeasure.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "SAssetDifferDataTableLayout"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDataTableLayout::Construct(const FArguments& InArgs)
{
	bIsLocal = InArgs._IsLocal;
	DataTableVisual = InArgs._DataTableVisual;
	InitialDesiredWidth = DataTableVisual->GetParentWindow()->GetClientSizeInScreen().X / 2;
	
	UAssetDifferDelegate::OnWindowResized.AddRaw(this, &SAssetDifferDataTableLayout::OnWindowResized);
	
	FText Title = InArgs._Title;

	if (bIsLocal)
	{
		DataTableVisual->DataTableLayoutLocal = SharedThis(this);
	}
	else
	{
		DataTableVisual->DataTableLayoutRemote = SharedThis(this);
	}

	SetupRowsData();
	
	SetupColumnWidth();
	RefreshRowNumberColumnWidth();
	RefreshRowNameColumnWidth();

	this->ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f)))
			.Visibility(EVisibility::SelfHitTestInvisible)
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
			.AutoHeight()
			[
				// Title
				SNew(STextBlock)
				.Text(Title)
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 13))
			]
			
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				BuildContent()
			]
		]
	];
}

void SAssetDifferDataTableLayout::SetupRowsData()
{
	AvailableColumns.Empty();
	AvailableRows.Empty();
	VisibleRows.Empty();
	
	DataTableVisual->GetDataTableData(bIsLocal, AvailableColumns, AvailableRows);

	// AvailableColumns.StableSort([](const FDataTableEditorColumnHeaderDataPtr& A, const FDataTableEditorColumnHeaderDataPtr& B)
	// {
	// 	return (A->ColumnId).ToString() > (B->ColumnId).ToString();
	// });
	
	for (auto RowData : AvailableRows)
	{
		FAssetDifferDataTableRowListViewDataPtr Row = MakeShared<FAssetDifferDataTableRowListViewData>();
		Row->CellData = RowData->CellData;
		Row->DisplayName = RowData->DisplayName;
		Row->RowId = RowData->RowId;
		Row->RowNum = RowData->RowNum;
		Row->DesiredRowHeight = RowData->DesiredRowHeight;
		VisibleRows.Add(Row);
	}
	
	ArrangeRows();
}

void SAssetDifferDataTableLayout::ArrangeRows()
{
	VisibleRows.StableSort([](const FAssetDifferDataTableRowListViewDataPtr& A, const FAssetDifferDataTableRowListViewDataPtr& B)
	{
		return (A->RowId).ToString() < (B->RowId).ToString();
	});

	for (int32 i = 0; i < VisibleRows.Num(); ++i)
	{
		if (VisibleRows[i]->RowNum == -2)
		{
			VisibleRows[i]->RowState = EDifferRowViewOption::Removed;
		}
		else if (VisibleRows[i]->RowNum == 0)
		{
			VisibleRows[i]->RowState = EDifferRowViewOption::Added;
		}
		else if (VisibleRows[i]->RowNum == -1)
		{
			VisibleRows[i]->RowState = EDifferRowViewOption::Modify;
		}
		
		VisibleRows[i]->RowNum = i + 1;
	}
	
	if (!DataTableVisual)
	{
		return;
	}

	VisibleRows.RemoveAll([this](const FAssetDifferDataTableRowListViewDataPtr& RowData)
	{
		if (DataTableVisual->HasRowViewOption(static_cast<EDifferRowViewOption>(RowData->RowState)))
		{
			return false;
		}
		return true;
	});
}

void SAssetDifferDataTableLayout::FilterRows(const FString& FilterName)
{
	VisibleRows.Empty();
	
	for (auto RowData : AvailableRows)
	{
		FAssetDifferDataTableRowListViewDataPtr Row = MakeShared<FAssetDifferDataTableRowListViewData>();
		Row->CellData = RowData->CellData;
		Row->DisplayName = RowData->DisplayName;
		Row->RowId = RowData->RowId;
		Row->RowNum = RowData->RowNum;
		Row->DesiredRowHeight = RowData->DesiredRowHeight;
		VisibleRows.Add(Row);
	}

	ArrangeRows();

	if (!FilterName.IsEmpty())
	{
		VisibleRows.RemoveAll([this, &FilterName](const FAssetDifferDataTableRowListViewDataPtr& RowData)
		{
			if (RowData->RowId.ToString().Find(FilterName) == INDEX_NONE)
			{
				return true;
			}

			return false;
		});
	}
	
	ListView->RequestListRefresh();
}

void SAssetDifferDataTableLayout::Refresh()
{
	if (ListView)
	{
		SetupRowsData();
		ListView->RequestListRefresh();
	}
}

TSharedRef<SWidget> SAssetDifferDataTableLayout::BuildContent()
{
	TSharedRef<SScrollBar> HorizontalScrollBar = SNew(SScrollBar)
	.Orientation(Orient_Horizontal)
	.Thickness(FVector2D(12.0f, 12.0f));

	TSharedRef<SScrollBar> VerticalScrollBar = SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(12.0f, 12.0f));

		
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = GenerateHeaderWidgets();
	ListView = SNew(SListView<FAssetDifferDataTableRowListViewDataPtr>)
		.ListItemsSource(&VisibleRows)
		.HeaderRow(ColumnNamesHeaderRow)
		.OnGenerateRow(this, &SAssetDifferDataTableLayout::MakeRowWidget)
		.OnListViewScrolled(this, &SAssetDifferDataTableLayout::OnListViewScrolled)
		.ExternalScrollbar(VerticalScrollBar)
		.ConsumeMouseWheel(EConsumeMouseWheel::Always)
		.SelectionMode(ESelectionMode::Single)
		.AllowOverscroll(EAllowOverscroll::Yes);
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(SScrollBox)
				.Orientation(Orient_Horizontal)
				.ExternalScrollbar(HorizontalScrollBar)
				+ SScrollBox::Slot()
				.HAlign(HAlign_Fill)
				.FillSize(1.f)
				[
					ListView.ToSharedRef()
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				VerticalScrollBar
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				HorizontalScrollBar
			]
		];
}

void SAssetDifferDataTableLayout::OnListViewScrolled(double InScrollOffset)
{
	if (DataTableVisual.IsValid())
	{
		DataTableVisual->SyncVerticalScrollOffset(bIsLocal, InScrollOffset);
	}
}

TSharedPtr<SHeaderRow> SAssetDifferDataTableLayout::GenerateHeaderWidgets()
{
	TSharedPtr<SHeaderRow> ColumnNamesHeaderRow = SNew(SHeaderRow);
	
	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowNumber"))
		.FillWidth(1.0f)
		.DefaultLabel(LOCTEXT("DataTableRowNumber", " "))
		.ManualWidth(this, &SAssetDifferDataTableLayout::GetRowNumberColumnWidth)
		.OnWidthChanged(this, &SAssetDifferDataTableLayout::OnRowNumberColumnResized)
	);

	ColumnNamesHeaderRow->AddColumn(
		SHeaderRow::Column(TEXT("RowName"))
		.DefaultLabel(LOCTEXT("DataTableRowName", "Row Name"))
		.ManualWidth(this, &SAssetDifferDataTableLayout::GetRowNameColumnWidth)
		.OnWidthChanged(this, &SAssetDifferDataTableLayout::OnRowNameColumnResized)
	);

	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];

		TAttribute<float> RowWidth = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateSP(this, &SAssetDifferDataTableLayout::GetColumnWidth, ColumnIndex));
		
		ColumnNamesHeaderRow->AddColumn(
			SHeaderRow::Column(ColumnData->ColumnId)
			.DefaultLabel(ColumnData->DisplayName)
			.ManualWidth(RowWidth)
			.OnWidthChanged(this, &SAssetDifferDataTableLayout::OnColumnResized, ColumnIndex)
			[
				SNew(SBox)
				.Padding(FMargin(0, 4, 0, 4))
				.VAlign(VAlign_Fill)
				.ToolTip(IDocumentation::Get()->CreateToolTip(FDataTableEditorUtils::GetRowTypeInfoTooltipText(ColumnData), nullptr, *FDataTableEditorUtils::VariableTypesTooltipDocLink, FDataTableEditorUtils::GetRowTypeTooltipDocExcerptName(ColumnData)))
				[
					SNew(STextBlock)
					.Justification(ETextJustify::Center)
					.Text(ColumnData->DisplayName)
					.ColorAndOpacity(this, &SAssetDifferDataTableLayout::GetHeaderColumnColorAndOpacity, ColumnIndex)
				]
			]
		);
	}
	
	return ColumnNamesHeaderRow;
}

FSlateColor SAssetDifferDataTableLayout::GetHeaderColumnColorAndOpacity(int32 ColumnIndex) const
{
	if (DataTableVisual)
	{
		return DataTableVisual->GetHeaderColumnColorAndOpacity(bIsLocal, ColumnIndex);
	}
	
	return FLinearColor(1.f, 1.f, 1.f);
}

void SAssetDifferDataTableLayout::SetupColumnWidth()
{
	// Setup the default auto-sized columns
	float TempSize = 0.f;
	ColumnWidths.SetNum(AvailableColumns.Num());
	for (int32 ColumnIndex = 0; ColumnIndex < AvailableColumns.Num(); ++ColumnIndex)
	{
		const FDataTableEditorColumnHeaderDataPtr& ColumnData = AvailableColumns[ColumnIndex];
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.CurrentWidth = FMath::Clamp(ColumnData->DesiredColumnWidth, 10.0f, 400.0f) + 10.f; // Clamp auto-sized columns to a reasonable limit

		TempSize += ColumnWidth.CurrentWidth;
		if (ColumnIndex == AvailableColumns.Num() - 1 && TempSize < InitialDesiredWidth)
		{
			ColumnWidth.CurrentWidth = InitialDesiredWidth - TempSize - 3;
		}
	}
}

float SAssetDifferDataTableLayout::GetRowNameColumnWidth() const
{
	return RowNameColumnWidth;
}

void SAssetDifferDataTableLayout::RefreshRowNameColumnWidth()
{
#if ENGINE_MAJOR_VERSION == 4
	const FTextBlockStyle& CellTextStyle = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#else
	const FTextBlockStyle& CellTextStyle = FAppStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#endif
	
	TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	static const float CellPadding = 10.0f;

	for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
	{
		const float RowNameWidth = (float)FontMeasure->Measure(RowData->DisplayName, CellTextStyle.Font).X + CellPadding;
		RowNameColumnWidth = FMath::Max(RowNameColumnWidth, RowNameWidth);
	}
}

void SAssetDifferDataTableLayout::RefreshRowNumberColumnWidth()
{
#if ENGINE_MAJOR_VERSION == 4
	const FTextBlockStyle& CellTextStyle = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#else
	const FTextBlockStyle& CellTextStyle = FAppStyle::GetWidgetStyle<FTextBlockStyle>("DataTableEditor.CellText");
#endif
	
	TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const float CellPadding = 10.0f;

	for (const FDataTableEditorRowListViewDataPtr& RowData : AvailableRows)
	{
		const float RowNumberWidth = (float)FontMeasure->Measure(FString::FromInt(RowData->RowNum), CellTextStyle.Font).X + CellPadding;
		RowNumberColumnWidth = FMath::Max(RowNumberColumnWidth, RowNumberWidth);
	}
}

float SAssetDifferDataTableLayout::GetColumnWidth(const int32 ColumnIndex) const
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		return ColumnWidths[ColumnIndex].CurrentWidth;
	}
	return 0.0f;
}

void SAssetDifferDataTableLayout::OnColumnResized(const float NewWidth, const int32 ColumnIndex)
{
	if (ColumnWidths.IsValidIndex(ColumnIndex))
	{
		FColumnWidth& ColumnWidth = ColumnWidths[ColumnIndex];
		ColumnWidth.bIsAutoSized = false;
		ColumnWidth.CurrentWidth = NewWidth;
	}
}

void SAssetDifferDataTableLayout::OnRowNameColumnResized(const float NewWidth)
{
	RowNameColumnWidth = NewWidth;
}

float SAssetDifferDataTableLayout::GetRowNumberColumnWidth() const
{
	return RowNumberColumnWidth;
}

void SAssetDifferDataTableLayout::OnRowNumberColumnResized(const float NewWidth)
{
	RowNumberColumnWidth = NewWidth;
}

TSharedRef<ITableRow> SAssetDifferDataTableLayout::MakeRowWidget(FAssetDifferDataTableRowListViewDataPtr InRowDataPtr,const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SAssetDifferDataTableListViewRow, OwnerTable).IsLocal(bIsLocal).DataTableVisual(DataTableVisual).InRowDataPtr(InRowDataPtr).DataTableLayout(SharedThis(this));
}

void SAssetDifferDataTableLayout::SetListViewScrollOffset(float InOffset)
{
	if (ListView.IsValid())
	{
		ListView->SetScrollOffset(InOffset);
	}
}

TSharedRef<SWidget> SAssetDifferDataTableLayout::CreateRowDetailView()
{
	return SNullWidget::NullWidget;
}

void SAssetDifferDataTableLayout::OnWindowResized(FVector2D WindowSize)
{
	InitialDesiredWidth = WindowSize.X / 2;
	SetupColumnWidth();
}

void SAssetDifferDataTableLayout::SelectRow(FName RowId)
{
	if (!ListView.IsValid())
	{
		return;
	}
	
	for (const auto& RowData : VisibleRows)
	{
		ListView->SetItemSelection(RowData, false);
	}
	
	for (const auto& RowData : VisibleRows)
	{
		if (RowData->RowId != RowId)
		{
			continue;
		}

		if (!ListView->IsItemSelected(RowData))
		{
			ListView->SetItemSelection(RowData, true);
			ListView->RequestScrollIntoView(RowData);
			break;
		}
	}
}

FText SAssetDifferDataTableLayout::GetCellText(const FName& InColumnId, const FName& RowName) const
{
	int32 ColumnIndex = -1;
	for (int32 i = 0; i < AvailableColumns.Num(); ++i)
	{
		if (AvailableColumns[i]->ColumnId == InColumnId)
		{
			ColumnIndex = i;
			break;
		}
	}
	
	if (ColumnIndex >= 0)
	{
		for (const auto& RowData : AvailableRows)
		{
			if (RowData->RowId == RowName)
			{
				if (RowData->CellData.IsValidIndex(ColumnIndex))
				{
					return RowData->CellData[ColumnIndex];
				}
			}
		}
	}
	
	return FText();
}

FSlateColor SAssetDifferDataTableLayout::GetCellTextColor(const FName& InColumnId, const FName& RowName) const
{
	for (const auto& RowData : VisibleRows)
	{
		if (RowName == RowData->RowId)
		{
			if (RowData->RowState == EDifferRowViewOption::Removed)
			{
				return FSlateColor(FLinearColor(0.8, 0.0, 0.1, 1.0));
			}
			else if (RowData->RowState == EDifferRowViewOption::Added)
			{
				return FSlateColor(FLinearColor(0.0, 0.8, 0.1, 1.0));
			}
			else if (RowData->RowState == EDifferRowViewOption::Modify)
			{
				return FSlateColor(FLinearColor(1.0, 1.0, 0.1, 1.0));
			}
		}
	}
	
	return FSlateColor(FLinearColor(0.5, 0.5, 0.5, 1.0));
}

bool SAssetDifferDataTableLayout::IsCellEnable(const FName& InColumnId, const FName& RowName) const
{
	return true;
}

TArray<FAssetDifferDataTableRowListViewDataPtr>& SAssetDifferDataTableLayout::GetVisibleRows()
{
	return VisibleRows;
}

TArray<FDataTableEditorColumnHeaderDataPtr>& SAssetDifferDataTableLayout::GetAvailableColumns()
{
	return AvailableColumns;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
