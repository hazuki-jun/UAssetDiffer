// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAssetDifferEditor/Base/AssetDifferSupportClassFactory.h"
#include "AssetDifferBlueprintFactory.generated.h"

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferBlueprintFactory : public UAssetDifferSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UAssetDifferBlueprintFactory() { SupportClassName = TEXT("Blueprint"); }

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferWidgetBlueprintFactory : public UAssetDifferSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UAssetDifferWidgetBlueprintFactory() { SupportClassName = TEXT("WidgetBlueprint"); }

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferAnimationBlueprintFactory : public UAssetDifferSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UAssetDifferAnimationBlueprintFactory() { SupportClassName = TEXT("AnimBlueprint"); }

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};
