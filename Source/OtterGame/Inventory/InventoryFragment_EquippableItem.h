// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Inventory/OtterInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UOtterEquipmentDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UOtterInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=Otter)
	TSubclassOf<UOtterEquipmentDefinition> EquipmentDefinition;
};
