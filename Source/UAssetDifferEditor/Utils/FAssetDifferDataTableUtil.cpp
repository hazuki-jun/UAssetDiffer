﻿#include "FAssetDifferDataTableUtil.h"

#include "Engine/DataTable.h"
#include "DataTableEditorUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "..\PropertyViewWidgets\AssetDifferDetailTreeNode.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FAssetDifferDataTableUtil"

void FAssetDifferDataTableUtil::ExportRowText(FString& ValueStr, UDataTable* DataTable, const FName& RowName)
{
	uint8* RowPtr = DataTable ? DataTable->GetRowMap().FindRef(RowName) : nullptr;
	if (!RowPtr)
	{
		return;
	}
	
	if (RowPtr)
	{
		DataTable->RowStruct->ExportText(ValueStr, RowPtr, RowPtr, DataTable, PPF_Copy, nullptr);
	}
}

void FAssetDifferDataTableUtil::ExportRowValueText(FString& ValueStr, UDataTable* DataTable, const FName& RowName)
{
	uint8* RowPtr = DataTable ? DataTable->GetRowMap().FindRef(RowName) : nullptr;
	if (!RowPtr)
	{
		return;
	}
	
	for (TFieldIterator<FProperty> It(DataTable->RowStruct); It; ++It)
	{
		if (!It->ShouldPort(PPF_Copy))
		{
			continue;
		}

		for (int32 Index = 0; Index < It->ArrayDim; Index++)
		{
			FString InnerValue;
			if (It->ExportText_InContainer(Index, InnerValue, RowPtr, RowPtr, DataTable, PPF_Delimited | PPF_Copy, nullptr))
			{
				ValueStr += InnerValue + ",";
			}
		}
	}

	ValueStr = ValueStr.Left(ValueStr.Len() - 1);
}

void FAssetDifferDataTableUtil::MergeRow(UDataTable* DestDataTable, UDataTable* SrcDataTable, const FName& RowName)
{
	if (!DestDataTable || !SrcDataTable || RowName.IsNone())
	{
		return;
	}

	FString SrcRow;
	ExportRowText(SrcRow, SrcDataTable, RowName);
	if (SrcRow.IsEmpty())
	{
		return;
	}

	if (!DestDataTable->GetRowMap().Find(RowName))
	{
		AddRow(DestDataTable, RowName);
	}
	
	uint8* RowPtr = DestDataTable ? DestDataTable->GetRowMap().FindRef(RowName) : nullptr;
	if (!RowPtr || !DestDataTable->RowStruct)
	{
		return;
	}

	const FScopedTransaction Transaction(LOCTEXT("PasteDataTableRow", "Paste Data Table Row"));
	DestDataTable->Modify();
	
	FDataTableEditorUtils::BroadcastPreChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	const TCHAR* Result = DestDataTable->RowStruct->ImportText(*SrcRow, RowPtr, DestDataTable, PPF_Copy, nullptr, GetPathNameSafe(DestDataTable->RowStruct));
	DestDataTable->HandleDataTableChanged(RowName);
	DestDataTable->MarkPackageDirty();
	FDataTableEditorUtils::BroadcastPostChange(DestDataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowData);
	
	if (Result == nullptr)
	{
		FNotificationInfo Info(LOCTEXT("FailedPaste", "Failed to paste row"));
		FSlateNotificationManager::Get().AddNotification(Info);
	}
}

void FAssetDifferDataTableUtil::AddRow(UDataTable* DestDataTable, const FName& RowName)
{
	FDataTableEditorUtils::AddRow(DestDataTable, RowName);
}

void FAssetDifferDataTableUtil::DeleteRow(UDataTable* DataTable, FName RowName)
{
	FDataTableEditorUtils::RemoveRow(DataTable, RowName);
}

bool FAssetDifferDataTableUtil::HasAnyDifferenceRowToRow(UDataTable* InLocalDataTable, UDataTable* InRemoteDataTable, FName& RowName, bool bIgnorePropertyName)
{
	FString LocalStructDataText;
	FString RemoteStructDataText;
	ExportRowText(LocalStructDataText, InLocalDataTable, RowName);
	ExportRowText(RemoteStructDataText, InRemoteDataTable, RowName);
	
	if (LocalStructDataText.IsEmpty() || RemoteStructDataText.IsEmpty())
	{
		return false;
	}
				
	if (RemoteStructDataText.Len() != LocalStructDataText.Len())
	{
		return true;
	}
				
	if (!RemoteStructDataText.Equals(LocalStructDataText, ESearchCase::IgnoreCase))
	{
		return true;
	}

	return false;
}

FString FAssetDifferDataTableUtil::CopyProperty(TSharedPtr<FAssetDifferDetailTreeNode> InNode)
{
	FString FormattedString;
	if (!InNode)
	{
		return FormattedString;
	}
	
	const void* ValueAddr = nullptr;
	if (InNode->IsInContainer())
	{
		ValueAddr = InNode->RawPtr; 
	}
	else
	{
		if (InNode->IsContainerNode())
		{
			ValueAddr = InNode->GetStructData();
		}
		else
		{
			void* StructData = InNode->GetStructData();
			ValueAddr = InNode->Property->ContainerPtrToValuePtr<void>(StructData); 
		}
	}
	
	InNode->Property.Get()->ExportText_Direct(FormattedString, ValueAddr, ValueAddr, nullptr, PPF_Copy);
	return FormattedString;
}

#undef LOCTEXT_NAMESPACE 
