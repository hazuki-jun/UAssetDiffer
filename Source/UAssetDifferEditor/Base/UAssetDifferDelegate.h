﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */

class UASSETDIFFEREDITOR_API UAssetDifferDelegate
{
public:
	DECLARE_DELEGATE(FOnBlueprintDiffWidgetClosed)
	static FOnBlueprintDiffWidgetClosed OnBlueprintDiffWidgetClosed;
	
	DECLARE_DELEGATE_ThreeParams(FOnDataTableRowSelected, bool, FName, int32)
	static FOnDataTableRowSelected OnDataTableRowSelected;

	DECLARE_DELEGATE_ThreeParams(FOnStringTableRowSelected, bool, FString, int32)
	static FOnStringTableRowSelected OnStringTableRowSelected;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnWindowResized, FVector2D)
	static FOnWindowResized OnWindowResized;
};
