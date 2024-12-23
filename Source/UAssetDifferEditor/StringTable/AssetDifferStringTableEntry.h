#pragma once
#include "UAssetDifferEditor/AssetDifferDefinitions.h"

/** Cached string table entry */
struct FCachedStringTableEntry
{
	FCachedStringTableEntry(FString InKey, FString InSourceString)
		: Key(MoveTemp(InKey))
		, SourceString(MoveTemp(InSourceString))
		, RowState(EDifferRowViewOption::Normal)
		, RowNumber(-1)
	{
	}

	FString Key;
	FString SourceString;
	EDifferRowViewOption RowState;
	int32 RowNumber;
};
