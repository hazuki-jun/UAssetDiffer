// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDiffWindow.h"

#include "DiffClassCollectionSubsystem.h"
#include "Styling/SlateStyle.h"

#include "SlateOptMacros.h"
#include "UAssetDifferDelegate.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "SBlueprintDiffWindow"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDiffWindow::Construct(const FArguments& InArgs)
{
	SWindow::Construct(SWindow::FArguments()
		.Title(InArgs._WindowTitel)
		.ClientSize(FVector2D(1000, 800)));
	
	auto DiffWidget = GetBlueprintDiffWidget(InArgs._LocalAsset, InArgs._RemoteAsset);
	SetContent(SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			DiffWidget
		]);

	WindowClosedEvent.AddLambda([](const TSharedRef<SWindow>& InWindow)
	{
		UAssetDifferDelegate::OnWindowResized.Clear();
		UAssetDifferDelegate::OnDataTableRowSelected.Unbind();
		UAssetDifferDelegate::OnStringTableRowSelected.Unbind();
	});
}

TSharedRef<SWidget> SAssetDiffWindow::GetBlueprintDiffWidget(UObject* LocalAsset, UObject* RemoteAsset)
{
	return GDiffClassCollectionSubsystem.CreateVisualDiffWidget(SharedThis(this), LocalAsset, RemoteAsset);
}

TSharedPtr<SAssetDiffWindow> SAssetDiffWindow::CreateWindow(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (!LocalAsset || !RemoteAsset)
	{
		return SNew(SAssetDiffWindow);
	}
	
	TSharedPtr<SAssetDiffWindow> Window =
		SNew(SAssetDiffWindow)
			.LocalAsset(LocalAsset)
			.RemoteAsset(RemoteAsset);
					
	
	TSharedPtr<SWindow> ActiveModal = FSlateApplication::Get().GetActiveModalWindow();
	if (ActiveModal.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), ActiveModal.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef());
	}

	return Window;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
