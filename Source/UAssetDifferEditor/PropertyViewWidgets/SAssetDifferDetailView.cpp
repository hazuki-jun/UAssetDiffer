// Fill out your copyright notice in the Description page of Project Settings.


#include "SAssetDifferDetailView.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetDifferDetailView::Construct(const FArguments& InArgs)
{
	/*
	ChildSlot
	[
		// Populate the widget
	];
	*/
}


void SAssetDifferDetailView::AppendCacheNodes(TArray<TSharedPtr<FAssetDifferDetailTreeNode>> InNodes)
{
	AllNodes.Append(InNodes);
}

void SAssetDifferDetailView::AddCacheNode(TSharedPtr<FAssetDifferDetailTreeNode> InNode)
{
	AllNodes.Add(InNode);
}

const TArray<TSharedPtr<FAssetDifferDetailTreeNode>>& SAssetDifferDetailView::GetCachedNodes()
{
	return AllNodes;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
