// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/OtterInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UOtterReticleWidgetBase;
class UObject;

UCLASS()
class UInventoryFragment_ReticleConfig : public UOtterInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Reticle)
	TArray<TSubclassOf<UOtterReticleWidgetBase>> ReticleWidgets;
};
