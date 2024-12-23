// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AssetDifferDefinitions.generated.h"

enum class EDifferRowViewOption : uint8
{
	Normal = 0x00000001,
	Modify = 0x00000002,
	Added = 0x00000004,
	Removed = 0x00000008,
	
	Max = Normal + Modify + Added + Removed
};
 
ENUM_CLASS_FLAGS(EDifferRowViewOption);

/**
 * 
 */
UCLASS()
class UASSETDIFFEREDITOR_API UUAssetDifferDefinitions : public UObject
{
	GENERATED_BODY()
};
