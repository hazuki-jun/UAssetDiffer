#include "FAssetDifferStringTableUtil.h"

#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

#define LOCTEXT_NAMESPACE "SAssetDifferStringTableEntryRow"

void FAssetDifferStringTableUtil::ModifyRow(UStringTable* StringTable, const FString& InKey, const FString& InSourceString)
{
	AddRow(StringTable, InKey, InSourceString);
}

bool FAssetDifferStringTableUtil::AddRow(class UStringTable* StringTable, const FString& InKey, const FString& InSourceString)
{
	if (!StringTable || InKey.IsEmpty() || InSourceString.IsEmpty())
	{
		return false;
	}
	
	if (StringTable)
	{
		const FScopedTransaction Transaction(LOCTEXT("SetStringTableEntry", "Set String Table Entry"));
		StringTable->Modify();
		StringTable->GetMutableStringTable()->SetSourceString(InKey, InSourceString);
	}

	return true;
}

void FAssetDifferStringTableUtil::DeleteRow(UStringTable* StringTable, const FString& InKey)
{
	if (!StringTable || InKey.IsEmpty())
	{
		return;
	}
	
	if (StringTable)
	{
		const FScopedTransaction Transaction(LOCTEXT("DeleteStringTableEntry", "Delete String Table Entry"));
		StringTable->Modify();
		StringTable->GetMutableStringTable()->RemoveSourceString(InKey);
	}
}

#undef LOCTEXT_NAMESPACE
