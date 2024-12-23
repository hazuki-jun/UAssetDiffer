// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetDifferClipboardData.h"

#include "EdGraphUtilities.h"

FAssetDifferClipboardData::FAssetDifferClipboardData()
{
}

FAssetDifferClipboardData::FAssetDifferClipboardData(const UEdGraph* InFuncGraph)
{
	SetFromGraph(InFuncGraph);
}

void FAssetDifferClipboardData::SetFromGraph(const UEdGraph* InFuncGraph)
{
	if (InFuncGraph)
	{
		OriginalBlueprint = Cast<UBlueprint>(InFuncGraph->GetOuter());

		GraphName = InFuncGraph->GetFName();

		if (const UEdGraphSchema* Schema = InFuncGraph->GetSchema())
		{
			GraphType = Schema->GetGraphType(InFuncGraph);
		}

		// TODO: Make this look nicer with an overload of ExportNodesToText that takes a TArray?
		// construct a TSet of the nodes in the graph for ExportNodesToText
		TSet<UObject*> Nodes;
		Nodes.Reserve(InFuncGraph->Nodes.Num());
		for (UEdGraphNode* Node : InFuncGraph->Nodes)
		{
			Nodes.Add(Node);
		}
		FEdGraphUtilities::ExportNodesToText(Nodes, NodesString);
	}
}
