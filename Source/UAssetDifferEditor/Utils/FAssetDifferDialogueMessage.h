#pragma once

#include "CoreMinimal.h"

class UASSETDIFFEREDITOR_API FAssetDifferDialogueMessage
{
public:
	static void ShowModal(const FText& Message, const FText& Title);
};
