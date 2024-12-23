#include "UAssetDifferEditor.h"


#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DesktopPlatformModule.h"
#include "EditorUtilityLibrary.h"
#include "IDesktopPlatform.h"
#include "ObjectTools.h"
#include "ToolMenus.h"
#include "Dialogs/Dialogs.h"
#include "Framework/Commands/GenericCommands.h"
#include "Interfaces/IPluginManager.h"
#include "UAssetDifferEditor/Base/DiffClassCollectionSubsystem.h"
#include "UAssetDifferEditor/Base/SAssetDiffWindow.h"
#include "UAssetDifferEditor/Base/UAssetDifferDelegate.h"

#define LOCTEXT_NAMESPACE "FUAssetDifferEditorModule"


void FUAssetDifferEditorModule::StartupModule()
{
	DeleteUAssets();
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
		BuildDiffAssetsMenu();
	}

	UAssetDifferDelegate::OnBlueprintDiffWidgetClosed.BindRaw(this, &FUAssetDifferEditorModule::OnDiffWindowClosed);
}

void FUAssetDifferEditorModule::ShutdownModule()
{
}

void FUAssetDifferEditorModule::BuildDiffAssetsMenu()
{
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
	FToolMenuSection& Section = Menu->AddSection("UAssetDifferOptions", LOCTEXT("UAssetDifferOptionsLabel", "Unreal Differ Plugin"));

	FToolMenuEntry EntryDiffAsset = FToolMenuEntry::InitMenuEntry(
		TEXT("Diff"),
		LOCTEXT("DiffAssetDiffLable","Diff"),
		LOCTEXT("DiffAssetDiff_ToolTip","Diff"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "SourceControl.Actions.Diff"),
		FUIAction(FExecuteAction::CreateLambda([this](){ OnOptionDiffClicked(); })));
	
	Section.AddEntry(EntryDiffAsset);
}

void FUAssetDifferEditorModule::OnOptionDiffClicked()
{
	static int32 DiffCount = 0;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return;
	}
	
	FString Filter = TEXT("uasset file|*.uasset");
	TArray<FString> OutFiles;
	UObject* RemoteAsset = nullptr;
	for (;;)
	{
		if (DesktopPlatform->OpenFileDialog(nullptr, TEXT("Choose Another File"), TEXT(""), TEXT(""), Filter, EFileDialogFlags::None, OutFiles))
		{
			if (OutFiles.Num() > 0)
			{
				FString DestFilePath = (FPaths::Combine(*FPaths::DiffDir(), TEXT("AssetToDiff_Right_"))) + FString::FromInt(DiffCount++);
				const FString& AssetExt = FPackageName::GetAssetPackageExtension();
				if (!DestFilePath.EndsWith(AssetExt))
				{
					DestFilePath += AssetExt;
				}
				
				if (IFileManager::Get().Copy(*DestFilePath, *OutFiles[0]) != COPY_OK)
				{
					UE_LOG(LogTemp, Error, TEXT("Copy failed: %s"), *DestFilePath)
					return;
				}
				
				RemoteAssetPath = DestFilePath;
				UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *DestFilePath, LOAD_None);
				if (AssetPkg)
				{
					RemoteAsset = AssetPkg->FindAssetInPackage();
				}
			}
		}
		break;
	}
	
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() > 0)
	{
		if (RemoteAsset)
		{
			ExecuteDiffAssets(SelectedAssets[0], RemoteAsset);
		}
	}
}

bool FUAssetDifferEditorModule::IsSupported()
{
	TArray<UObject*> SelectedAssets =  UEditorUtilityLibrary::GetSelectedAssets();
	if (SelectedAssets.Num() <= 0 || !SelectedAssets[0])
	{
		return false;
	}

	if (!GDiffClassCollectionSubsystem.IsSupported(SelectedAssets[0]))
	{
		return false;
	}

	return true;
}

void FUAssetDifferEditorModule::OnDiffWindowClosed()
{
	// DeleteLoadedUAssets();
}

void FUAssetDifferEditorModule::DeleteLoadedUAssets()
{
	if (RemoteAssetPath.IsEmpty())
	{
		return;
	}
	
	UPackage* AssetPkg = LoadPackage(/*Outer =*/nullptr, *RemoteAssetPath, LOAD_None);
	if (AssetPkg)
	{
		UObject* RemoteAsset = AssetPkg->FindAssetInPackage();
		if (RemoteAsset)
		{
			ObjectTools::DeleteObjectsUnchecked({RemoteAsset});
			RemoteAssetPath = "";
		}
	}
}

void FUAssetDifferEditorModule::DeleteUAssets()
{
	const auto Path = FPaths::Combine(*FPaths::DiffDir());
	if (!FPaths::DirectoryExists(Path))
	{
		return;
	}
	
	TArray<FString> Files;
	
	auto SearchSuffixFiles = [this, &Files, &Path](const FString& Suffix)
	{
		IFileManager::Get().FindFilesRecursive(Files, *Path, *Suffix, true, false, false);
	};
	
	for (const FString& Suffix : {TEXT("*.uasset")})
	{
		SearchSuffixFiles(Suffix);
	}
	
	for (const auto& File : Files)
	{
		if (FPaths::FileExists(File))
		{
			IFileManager::Get().Delete(*File, true);
		}
	}
}

void FUAssetDifferEditorModule::ExecuteDiffAssets(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (LocalAsset->GetClass() == RemoteAsset->GetClass())
	{
		if (!IsSupported())
		{
			FRevisionInfo RevisionInfo;
			RevisionInfo.Revision = TEXT("");
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
			AssetToolsModule.Get().DiffAssets(LocalAsset, RemoteAsset, RevisionInfo, RevisionInfo);
			return;
		}
	}
	else
	{
		FText NotSupportWarning = LOCTEXT("NotSupportWarningMessage", "类型不一致");
		FSuppressableWarningDialog::FSetupInfo Info( NotSupportWarning, LOCTEXT("NotSupport_Message", "Not Support"), "NotSupport_Warning" );
		Info.ConfirmText = LOCTEXT( "NotSupport_Yes", "Ok");
		Info.CancelText = LOCTEXT( "NotSupport_No", "Cancel");	
		FSuppressableWarningDialog RemoveLevelWarning( Info );
		auto Result = RemoveLevelWarning.ShowModal();
		return;
	}

	PerformDiffAction(LocalAsset, RemoteAsset);
}

void FUAssetDifferEditorModule::PerformDiffAction(UObject* LocalAsset, UObject* RemoteAsset)
{
	if (!LocalAsset || !RemoteAsset)
	{
		return;
	}
	
	auto BlueprintDiffWindow = SAssetDiffWindow::CreateWindow(LocalAsset, RemoteAsset);
}

#undef LOCTEXT_NAMESPACE

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FUAssetDifferEditorModule, UAssetDifferEditor)