// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDifferStringTableFactory.h"

#include "SStringTableVisualDiff.h"


TSharedRef<SWidget> UAssetDifferStringTableFactory::FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNew(SStringTableVisualDiff)
		.ParentWindow(ParentWindow)
		.LocalAsset(InLocalAsset)
		.RemoteAsset(InRemoteAsset);
}
