// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/Abilities/OtterGameplayAbility.h"

#include "OtterGameplayAbility_FromEquipment.generated.h"

class UOtterEquipmentInstance;
class UOtterInventoryItemInstance;

/**
 * UOtterGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class UOtterGameplayAbility_FromEquipment : public UOtterGameplayAbility
{
	GENERATED_BODY()

public:

	UOtterGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Otter|Ability")
	UOtterEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|Ability")
	UOtterInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
