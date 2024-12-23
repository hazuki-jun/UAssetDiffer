#pragma once

#include "CoreMinimal.h"

struct UASSETDIFFEREDITOR_API FAssetDifferStringTableUtil
{
public:
	/**
	 * @brief 修改 SourceString
	 * @param StringTable 
	 * @param InKey 
	 * @param InSourceString 
	 */
	static void ModifyRow(class UStringTable* StringTable, const FString& InKey, const FString& InSourceString);

	/**
	 * @brief 添加一行
	 * @param StringTable 
	 * @param InKey 
	 * @param InSourceString 
	 */
	static bool AddRow(class UStringTable* StringTable, const FString& InKey, const FString& InSourceString);

	/**
	 * @brief 删除一行
	 * @param StringTable 
	 * @param InKey 
	 */
	static void DeleteRow(class UStringTable* StringTable, const FString& InKey);
};


