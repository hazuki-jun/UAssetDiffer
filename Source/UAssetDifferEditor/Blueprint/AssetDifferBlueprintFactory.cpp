// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDifferBlueprintFactory.h"

#include "SBlueprintVisualDiff.h"

TSharedRef<SWidget> UAssetDifferBlueprintFactory::FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}

TSharedRef<SWidget> UAssetDifferWidgetBlueprintFactory::FactoryCreateVisualWidget(
	TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}

TSharedRef<SWidget> UAssetDifferAnimationBlueprintFactory::FactoryCreateVisualWidget(
	TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SBlueprintVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
