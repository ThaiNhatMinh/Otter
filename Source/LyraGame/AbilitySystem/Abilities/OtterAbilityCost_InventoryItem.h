// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OtterAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "OtterAbilityCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UOtterGameplayAbility;
class UOtterInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class UOtterAbilityCost_InventoryItem : public UOtterAbilityCost
{
	GENERATED_BODY()

public:
	UOtterAbilityCost_InventoryItem();

	//~UOtterAbilityCost interface
	virtual bool CheckCost(const UOtterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UOtterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UOtterAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<UOtterInventoryItemDefinition> ItemDefinition;
};
