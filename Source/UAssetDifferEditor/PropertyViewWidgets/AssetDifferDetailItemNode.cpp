#include "AssetDifferDetailItemNode.h"

#include "ObjectEditorUtils.h"

#include "SAssetDifferDetailSingleItemRow.h"
#include "SAssetDifferDetailView.h"

TSharedRef<ITableRow> FAssetDifferDetailItemNode::GenerateWidgetForTableView(const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SAssetDifferDetailSingleItemRow, OwnerTable, AsShared());
}

void FAssetDifferDetailItemNode::GetChildren(TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& OutChildren)
{
	// for (auto Child : Children)
	// {
	// 	OutChildren.Add(Child);
	// }
	OutChildren = Children;
}

SAssetDifferDetailView* FAssetDifferDetailItemNode::GetDetailsView() const
{
	return DetailView;
}

FText FAssetDifferDetailItemNode::GetDisplayNameText()
{
	return DisplayNameText; 
}

FString FAssetDifferDetailItemNode::GetPropertyValueAsString()
{
	return ValueText.ToString();
}

FText FAssetDifferDetailItemNode::GetValueText()
{
	return ValueText;
}

void FAssetDifferDetailItemNode::GenerateChildren()
{
	if (IsMapKeyOrValue())
	{
		GenerateMapKeyValueChildren();
		return;
	}
	
	auto CreateChildItemNode = [this](TWeakFieldPtr<const class FProperty> InProperty, bool bAddToCache)
	{
		TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = MakeShareable(new FAssetDifferDetailItemNode(DetailView));
		if (bAddToCache)
		{
			DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());	
			DetailView->AddCacheNode(DetailItemNode);
		}
		
		DetailItemNode->ParentNode = AsShared();
		DetailItemNode->Property = InProperty;

		return DetailItemNode;
	};
	
	if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
	{
		for (TFieldIterator<FProperty> It(StructProp->Struct); It; ++It)
		{
			TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = CreateChildItemNode(*It, true);
			DetailItemNode->ValueText = GetValueTextStructInternal(*It);
			DetailItemNode->DisplayNameText = It->GetDisplayNameText();
			DetailItemNode->SetNodeId(*It->GetName());
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
	else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		FScriptArrayHelper ArrayHelper(ArrayProp, StructData);
		for (int32 ArrayEntryIndex = 0; ArrayEntryIndex < ArrayHelper.Num(); ++ArrayEntryIndex)
		{
			TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = CreateChildItemNode(ArrayProp->Inner, true);
			DetailItemNode->RawPtr = ArrayHelper.GetRawPtr(ArrayEntryIndex);
			DetailItemNode->ContainerIndex = ArrayEntryIndex;
			DetailItemNode->ValueText = GetValueTextInternal(DetailItemNode->RawPtr, ArrayProp->Inner);
			DetailItemNode->DisplayNameText = FText::FromString(FString::Format(TEXT("[{0}]"), {ArrayEntryIndex}));
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(ArrayEntryIndex));
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
	else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		FScriptSetHelper SetHelper(SetProp, StructData);
		for (int32 SetSparseIndex = 0; SetSparseIndex < SetHelper.GetMaxIndex(); ++SetSparseIndex)
		{
			if (!SetHelper.IsValidIndex(SetSparseIndex))
			{
				return;
			}

			TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = CreateChildItemNode(SetHelper.GetElementProperty(), true);
			DetailItemNode->RawPtr = SetHelper.GetElementPtr(SetSparseIndex);
			DetailItemNode->ContainerIndex = SetSparseIndex;
			DetailItemNode->ValueText = GetValueTextInternal(DetailItemNode->RawPtr, SetHelper.GetElementProperty());
			DetailItemNode->DisplayNameText = FText::FromString(FString::Format(TEXT("[{0}]"), {SetSparseIndex}));
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(SetSparseIndex));
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
	else if (const FMapProperty* MapProp = CastField<FMapProperty>(Property.Get()))
	{
		auto StructData = GetStructData(0);
		void* RowData = MapProp->ContainerPtrToValuePtr<void>(StructData);
		FScriptMapHelper MapHelper(MapProp, RowData);
		for (int32 MapSparseIndex = 0; MapSparseIndex < MapHelper.GetMaxIndex(); ++MapSparseIndex)
		{
			TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = CreateChildItemNode(MapProp, true);
			DetailItemNode->RawPtr = (uint8*)RowData;
			DetailItemNode->ContainerIndex = MapSparseIndex;
			DetailItemNode->bIsMapElement = true;
			DetailItemNode->DisplayNameText = FText::FromString(FString::Format(TEXT("Element [{0}]"), {MapSparseIndex}));
			DetailItemNode->SetNodeId(DetailItemNode->Property->GetName() + FString::FromInt(MapSparseIndex));
			DetailItemNode->GenerateChildren();
			Children.Add(DetailItemNode);
		}
	}
}

void FAssetDifferDetailItemNode::GenerateMapKeyValueChildren()
{
	auto MapProp = CastField<FMapProperty>(Property.Get());
	if (!MapProp)
	{
		return;
	}

	auto CreateChildItemNode = [this](TWeakFieldPtr<const class FProperty> InProperty, bool bAddToCache)
	{
		TSharedPtr<FAssetDifferDetailItemNode> DetailItemNode = MakeShareable(new FAssetDifferDetailItemNode(DetailView));
		if (bAddToCache)
		{
			DetailItemNode->SetNodeIndex(DetailView->GetCachedNodeNum());	
			DetailView->AddCacheNode(DetailItemNode);
		}
		
		DetailItemNode->ParentNode = AsShared();
		DetailItemNode->Property = InProperty;

		return DetailItemNode;
	};


	FScriptMapHelper MapHelper(MapProp, RawPtr);
	if (!MapHelper.IsValidIndex(ContainerIndex))
	{
		return;
	}
	
	TSharedPtr<FAssetDifferDetailItemNode> DetailItemNodeMapKey = CreateChildItemNode( MapHelper.GetKeyProperty(), true);
	DetailItemNodeMapKey->RawPtr = MapHelper.GetKeyPtr(ContainerIndex);
	DetailItemNodeMapKey->DisplayNameText = FText::FromString(FString::Format(TEXT("Key({0})"), { MapHelper.GetKeyProperty()->GetDisplayNameText().ToString()}));
	DetailItemNodeMapKey->ContainerIndex = 0;
	DetailItemNodeMapKey->ValueText = GetValueTextInternal(DetailItemNodeMapKey->RawPtr, MapHelper.GetKeyProperty());
	DetailItemNodeMapKey->SetNodeId(FString(TEXT("MapKey_"))  + DetailItemNodeMapKey->Property->GetName());
	DetailItemNodeMapKey->GenerateChildren();
	Children.Add(DetailItemNodeMapKey);

	TSharedPtr<FAssetDifferDetailItemNode> DetailItemNodeMapValue = CreateChildItemNode(MapHelper.GetValueProperty(), true);
	DetailItemNodeMapValue->RawPtr = MapHelper.GetValuePtr(ContainerIndex);
	DetailItemNodeMapValue->DisplayNameText = FText::FromString(FString::Format(TEXT("Value({0})"), {MapHelper.GetValueProperty()->GetDisplayNameText().ToString()}));
	DetailItemNodeMapValue->ContainerIndex = 1;
	FString ValueStr = DataTableUtils::GetPropertyValueAsStringDirect(MapHelper.GetValueProperty(), DetailItemNodeMapValue->RawPtr, EDataTableExportFlags::UseJsonObjectsForStructs);;
	DetailItemNodeMapValue->ValueText = FText::FromString(ValueStr);
	DetailItemNodeMapValue->SetNodeId(FString(TEXT("MapValue_"))  + DetailItemNodeMapValue->Property->GetName());
	DetailItemNodeMapValue->GenerateChildren();
	Children.Add(DetailItemNodeMapValue);
}

void* FAssetDifferDetailItemNode::GetStructData(int32 ArrayIndex)
{
	if (IsInContainer())
	{
		return RawPtr;
	}
	
	if (ParentNode.IsValid())
	{
		auto ParentStructData = ParentNode.Pin()->GetStructData();

		if (const FStructProperty* StructProp = CastField<FStructProperty>(Property.Get()))
		{
			return StructProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
		}
		else if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property.Get()))
		{
			if (ParentStructData)
			{
				return ArrayProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
			}
		}
		else if (const FSetProperty* SetProp = CastField<FSetProperty>(Property.Get()))
		{
			if (ParentStructData)
			{
				return SetProp->ContainerPtrToValuePtr<void>(ParentStructData, ArrayIndex);
			}
		}
		
		return ParentStructData;
	}
	
	return nullptr;
}

FString FAssetDifferDetailItemNode::GetParentUniqueNodeId() const
{
	if (ParentNode.IsValid())
	{
		return ParentNode.Pin()->GetUniqueNodeId();
	}

	return FString();
}

