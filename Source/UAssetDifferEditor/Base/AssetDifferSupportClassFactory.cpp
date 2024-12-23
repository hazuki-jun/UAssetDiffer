// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDifferSupportClassFactory.h"

UAssetDifferSupportClassFactory::UAssetDifferSupportClassFactory()
{
	
}

FSuppressableWarningDialog::FSetupInfo UAssetDifferSupportClassFactory::GetDiffFailedDialogInfo()
{
	return FSuppressableWarningDialog::FSetupInfo(FText(), FText(), FString());
}

bool UAssetDifferSupportClassFactory::CanDiff(UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return true;
}

FName UAssetDifferSupportClassFactory::GetSupportedClass()
{
	return SupportClassName;
}

TSharedRef<SWidget> UAssetDifferSupportClassFactory::FactoryCreateVisualWidget(TSharedPtr<class SWindow> ParentWindow, UObject* InLocalAsset, UObject* InRemoteAsset)
{
	return SNullWidget::NullWidget;
}
