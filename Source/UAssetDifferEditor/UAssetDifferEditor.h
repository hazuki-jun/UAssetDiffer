﻿#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FUAssetDifferEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
 
    void BuildDiffAssetsMenu();

    // Diff
    void OnOptionDiffClicked();
 
    // 是否支持对比的资源
    bool IsSupported();

    void OnDiffWindowClosed();
 
    void DeleteLoadedUAssets();

    /**
     * @brief 启动时删除Diff目录下所有.uasset文件
     */
    void DeleteUAssets();

    /**
     * @brief 对比2个资源 同时检查是否受支持的
     * @param AssetA 
     * @param AssetB 
     */
    void ExecuteDiffAssets(UObject* AssetA, UObject* AssetB);

    /**
     * @brief 对比2个资源 不检查
     * @param AssetA 
     * @param AssetB 
     */
    void PerformDiffAction(UObject* AssetA, UObject* AssetB);

    FString RemoteAssetPath;

    static inline FUAssetDifferEditorModule& Get()
    {
        static FName AssetDifferModule("UAssetDifferEditor");
        return FModuleManager::LoadModuleChecked<FUAssetDifferEditorModule>(AssetDifferModule);
    }

public:
    virtual void Extern_ExecuteDiffAssets(UObject* AssetA, UObject* AssetB)
    {
        ExecuteDiffAssets(AssetA, AssetB);
    }
};
