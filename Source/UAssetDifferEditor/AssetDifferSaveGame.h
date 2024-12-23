// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AssetDifferSaveGame.generated.h"

USTRUCT()
struct FAssetDifferExpandedStruct
{
	GENERATED_BODY()

	UPROPERTY()
	TSet<FName> ExpandedCategoryName;
};

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UAssetDifferSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UAssetDifferSaveGame();
	
	static UAssetDifferSaveGame* GetSaveGame();
	
	static bool IsRowCategoryExpanded(const FName& RowName, FName CategoryName);

	static void AddExpandedCategory(const FName& RowName, FName CategoryName);

	static void RemoveExpandedCategory(const FName& RowName, FName CategoryName);
	
	static void Save(UAssetDifferSaveGame* InSaveGame);
	
	UPROPERTY()
	TMap<FName, FAssetDifferExpandedStruct> DataTableRowExpandedMap;

public:
	static void PropertyExtension_AddBlueprintStringTableKeyPrefix(const FName& InBlueprintName, const FString& InPrefix);
	
	static FString PropertyExtension_GetBlueprintStringTableKeyPrefix(const FName& InBlueprintName);

	void PropertyExtension_SetDefaultGlobalStringTableKeyPrefix(const FString& InPrefix);

	UPROPERTY()
	TMap<FName, FString> PropertyExtension_StringTableKeyPrefix;
	
	UPROPERTY(EditAnywhere, Category = WidgetBlueprint)
	FString DefaultGlobalStringTableKeyPrefix = TEXT("DefaultKey");
	
public:
	static FString PropertyExtension_GetDefaultGlobalStringTable();
	
	static void PropertyExtension_SetDefaultGlobalStringTable(TSoftObjectPtr<class UStringTable> InDefaultGlobalStringTable);
	
	static void PropertyExtension_AddStringTable(const FName& BlueprintName, const FString& StringTablePath);
	
	static FString PropertyExtension_GetStringTable(const FName& BlueprintName);

	UPROPERTY()
	TMap<FName, FString> PropertyExtension_DefaultStringTables;
	
	UPROPERTY(EditAnywhere, Category = WidgetBlueprint)
	TSoftObjectPtr<class UStringTable> DefaultGlobalStringTable;
};
