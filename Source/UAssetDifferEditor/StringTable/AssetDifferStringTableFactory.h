// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\Base\AssetDifferSupportClassFactory.h"
#include "AssetDifferStringTableFactory.generated.h"

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferStringTableFactory : public UAssetDifferSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UAssetDifferStringTableFactory() { SupportClassName = TEXT("StringTable"); }

	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};