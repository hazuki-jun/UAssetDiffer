// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UAssetDifferEditor/Base/AssetDifferSupportClassFactory.h"
#include "AssetDifferDataTableFactory.generated.h"

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferDataTableFactory : public UAssetDifferSupportClassFactory
{
	GENERATED_BODY()
	
public:
	UAssetDifferDataTableFactory() { SupportClassName = TEXT("DataTable"); }

	virtual bool CanDiff(UObject* InLocalAsset, UObject* InRemoteAsset) override;

	virtual FSuppressableWarningDialog::FSetupInfo GetDiffFailedDialogInfo() override;
	
	virtual TSharedRef<SWidget> FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset) override;
};
