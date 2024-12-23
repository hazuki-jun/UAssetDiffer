#include "AssetDifferDetailTreeNode.h"

#if ENGINE_MAJOR_VERSION == 4
	#include "WeakFieldPtr.h"
#endif

FAssetDifferDetailTreeNode::FAssetDifferDetailTreeNode()
{
}

FAssetDifferDetailTreeNode::~FAssetDifferDetailTreeNode()
{
}

bool FAssetDifferDetailTreeNode::IsMapElement() const
{
	return bIsMapElement;
}

bool FAssetDifferDetailTreeNode::IsContainerNode() const
{
	if (CastField<FStructProperty>(Property.Get()))
	{
		return true;
	}
	
	if (CastField<FMapProperty>(Property.Get()))
	{
		return true;
	}

	if (CastField<FArrayProperty>(Property.Get()))
	{
		return true;
	}

	if (CastField<FSetProperty>(Property.Get()))
	{
		return true;
	}
	
	return Children.Num() > 0;
}

bool FAssetDifferDetailTreeNode::IsMapKeyOrValue()
{
	if (Property.Get())
	{
		return CastField<FMapProperty>(Property.Get()) && ContainerIndex >= 0;
	}

	return false;
}

FString FAssetDifferDetailTreeNode::GetPropertyValueAsString()
{
	return FString();
}

FText FAssetDifferDetailTreeNode::GetValueText()
{
	
	return FText();
}

FText FAssetDifferDetailTreeNode::GetDisplayNameText()
{
	return FText();
}

FString FAssetDifferDetailTreeNode::GetUniqueNodeId() const
{
	return NodeId + GetParentUniqueNodeId();
}

FString FAssetDifferDetailTreeNode::GetParentUniqueNodeId() const
{
	return FString();
}

FName FAssetDifferDetailTreeNode::GetCategoryName()
{
	if (Property.IsValid() && IsContainerNode())
	{
		return Property->GetFName();
	}

	return NAME_None;
}

bool FAssetDifferDetailTreeNode::HasAnyDifferenceRecurse()
{
	bool Ret = bHasAnyDifference;
	if (Children.Num() > 0)
	{
		for (const auto& TreeNode : Children)
		{
			if (TreeNode->HasAnyDifferenceRecurse())
			{
				Ret = true;
			}
		}		
	}
	
	return Ret;
}

const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& FAssetDifferDetailTreeNode::GetChildNodes()
{
	return Children;
}

FText FAssetDifferDetailTreeNode::GetValueTextStructInternal(const FProperty* InProperty)
{
	const auto StructData = GetStructData(0);
	// void* RowData = InProperty->ContainerPtrToValuePtr<void>(StructData, 0)
	return GetValueTextInternal(StructData, InProperty);
}

FText FAssetDifferDetailTreeNode::GetValueTextContainerInternal(void* PropertyData, const FProperty* InProperty)
{
	return GetValueTextInternal(PropertyData, InProperty);
}

FText FAssetDifferDetailTreeNode::GetValueTextInternal(void* ValueAddress, const FProperty* InProperty)
{
	FText OutText;
	if (CastField<const FStructProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FMapProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FSetProperty>(InProperty))
	{
		return OutText;
	}
	else if (CastField<const FArrayProperty>(InProperty))
	{
		return OutText;
	}
	
	EDataTableExportFlags DTExportFlags = EDataTableExportFlags::UseJsonObjectsForStructs;
	OutText = DataTableUtils::GetPropertyValueAsText(InProperty, (uint8*)ValueAddress);
	if (OutText.IsEmpty())
	{
		OutText = FText::FromString("None");
	}
	
	return OutText;
}

