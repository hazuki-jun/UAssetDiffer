// Fill out your copyright notice in the Description page of Project Settings.


#include "SVisualDiffWidget.h"

#include "SlateOptMacros.h"
#include "Widgets/Images/SLayeredImage.h"
#include "Widgets/Input/SSearchBox.h"

#define LOCTEXT_NAMESPACE "SVisualDiffWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SVisualDiffWidget::Construct(const FArguments& InArgs)
{
}

TSharedRef<SWidget> SVisualDiffWidget::MakeToolbar()
{
	TSharedRef<SOverlay> Overlay = SNew(SOverlay);
	
	TSharedRef<SVerticalBox> OutVerticalBox =
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.AutoHeight()
		[
			Overlay
		];
	
	if (IsSearchable())
	{
		OutVerticalBox->AddSlot()
		.HAlign(HAlign_Fill)
		.AutoHeight()
		[
			SAssignNew(SearchBoxWidget, SSearchBox)
			.InitialText(this, &SVisualDiffWidget::GetFilterText)
			.OnTextChanged(this, &SVisualDiffWidget::OnFilterTextChanged)
			.OnTextCommitted(this, &SVisualDiffWidget::OnFilterTextCommitted)
		];
	}
	
	FToolBarBuilder DataTableVisualToolbarBuilder(TSharedPtr< const FUICommandList >(), FMultiBoxCustomization::None);
	DataTableVisualToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_HighlightPrevDifference))
		, NAME_None
		, LOCTEXT("PrevLabel", "Prev")
		, LOCTEXT("PrevTooltip", "Prev Difference")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintMerge.PrevDiff")
	);

	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_HighlightNextDifference))
		, NAME_None
		, LOCTEXT("Nextabel", "Next")
		, LOCTEXT("NextTooltip", "Next Difference")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintMerge.NextDiff")
	);
	
	if (IsDiffable())
	{
			DataTableVisualToolbarBuilder.AddToolBarButton(
		FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_Diff))
			, NAME_None
			, LOCTEXT("DiffLabel", "Diff")
			, LOCTEXT("DiffTooltip", "Diff")
			, FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff")
		);
	}
	
	DataTableVisualToolbarBuilder.AddToolBarButton(
	FUIAction(FExecuteAction::CreateSP(this, &SVisualDiffWidget::ToolbarAction_Merge))
		, NAME_None
		, LOCTEXT("MergeLabel", "Merge")
		, LOCTEXT("MergeTooltip", "Merge")
		, FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentReference.UseSelectionFromContentBrowser")
	);
	
	Overlay->AddSlot()
	.HAlign(HAlign_Left)
	.Padding(FMargin(10.f, 0.f, 0.f, 0.f))
	[
		DataTableVisualToolbarBuilder.MakeWidget()
	];
	
	// View option
	TSharedPtr<SLayeredImage> FilterImage =
		SNew(SLayeredImage)
		.Image(FAppStyle::Get().GetBrush("DetailsView.ViewOptions"))
		.ColorAndOpacity(FSlateColor::UseForeground());

	TSharedRef<SComboButton> ComboButton =
		SNew(SComboButton)
		.HasDownArrow(false)
		.ContentPadding(0)
		.ForegroundColor(FSlateColor::UseForeground())
		.ButtonStyle(FAppStyle::Get(), "SimpleButton")
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("ViewOptions")))
		.MenuContent()
		[
			GetShowViewOptionContent()
		]
		.ButtonContent()
		[
			FilterImage.ToSharedRef()
		];
	
	Overlay->AddSlot()
	.HAlign(HAlign_Center)
	.Padding(FMargin(0.f, 0.f, 20.f, 0.f))
	[
		ComboButton
	];
	
	return OutVerticalBox;
}

FText SVisualDiffWidget::GetFilterText() const
{
	return ActiveFilterText;
}

void SVisualDiffWidget::OnFilterTextChanged(const FText& InFilterText)
{
	ActiveFilterText = InFilterText;
	UpdateVisibleRows();
}

void SVisualDiffWidget::OnFilterTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (CommitInfo == ETextCommit::OnCleared)
	{
		SearchBoxWidget->SetText(FText::GetEmpty());
		OnFilterTextChanged(FText::GetEmpty());
	}
}

void SVisualDiffWidget::UpdateVisibleRows()
{
}

void SVisualDiffWidget::ToolbarAction_HighlightNextDifference()
{
}

void SVisualDiffWidget::ToolbarAction_HighlightPrevDifference()
{
}

bool SVisualDiffWidget::IsDiffable()
{
	return true;
}

void SVisualDiffWidget::ToolbarAction_Diff()
{
}

bool SVisualDiffWidget::ToolbarAction_CanDiff()
{
	return false;
}

void SVisualDiffWidget::ToolbarAction_Merge()
{
}

bool SVisualDiffWidget::IsSearchable()
{
	return true;
}

TSharedRef<SWidget> SVisualDiffWidget::GetShowViewOptionContent()
{
	FMenuBuilder DetailViewOptions( true, nullptr);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyNormal", "Show Only Normal Rows"),
	LOCTEXT("ShowOnlyNormal_ToolTip", "Displays only row"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EDifferRowViewOption::Normal),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EDifferRowViewOption::Normal )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyModified", "Show Only Modified Rows"),
	LOCTEXT("ShowOnlyModified_ToolTip", "Displays only row which have been changed"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EDifferRowViewOption::Modify),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EDifferRowViewOption::Modify )
			),
		NAME_None,
		EUserInterfaceActionType::Check);

	DetailViewOptions.AddMenuEntry( 
	LOCTEXT("ShowOnlyAdded", "Show Only Added Rows"),
	LOCTEXT("ShowOnlyAdded_ToolTip", "Displays only row which added"),
		FSlateIcon(),
		FUIAction(
		FExecuteAction::CreateSP(this, &SVisualDiffWidget::OnShowOnlyViewOptionClicked, EDifferRowViewOption::Added),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP( this, &SVisualDiffWidget::IsShowOnlyRowViewOptionChecked, EDifferRowViewOption::Added )
			),
		NAME_None,
		EUserInterfaceActionType::Check);
	
	return DetailViewOptions.MakeWidget();
}

void SVisualDiffWidget::OnShowOnlyViewOptionClicked(EDifferRowViewOption InViewOption)
{
	if (InViewOption == EDifferRowViewOption::Added)
	{
		ReverseRowViewOption(EDifferRowViewOption::Added);
		ReverseRowViewOption(EDifferRowViewOption::Removed);
	}
	else
	{
		ReverseRowViewOption(InViewOption);
	}
	
	RefreshLayout();
	ModifyConfig();
}

void SVisualDiffWidget::RefreshLayout()
{
	
}

bool SVisualDiffWidget::HasRowViewOption(EDifferRowViewOption InViewOption) const
{
	return static_cast<int32>((RowViewOption & InViewOption)) != 0;
}

void SVisualDiffWidget::ClearRowViewOption(EDifferRowViewOption InViewOption)
{
	RowViewOption = RowViewOption & ~InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SVisualDiffWidget::SetRowViewOption(EDifferRowViewOption InViewOption)
{
	RowViewOption = RowViewOption | InViewOption;
	SetRowViewOptionTo(RowViewOption);
}

void SVisualDiffWidget::SetRowViewOptionTo(EDifferRowViewOption InViewOption)
{
	RowViewOption = InViewOption;
}

void SVisualDiffWidget::ReverseRowViewOption(EDifferRowViewOption InViewOption)
{
	HasRowViewOption(InViewOption) ? ClearRowViewOption(InViewOption) : SetRowViewOption(InViewOption);
}

void SVisualDiffWidget::ModifyConfig()
{

}

bool SVisualDiffWidget::IsShowOnlyRowViewOptionChecked(EDifferRowViewOption InViewOption) const
{
	return HasRowViewOption(InViewOption);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
