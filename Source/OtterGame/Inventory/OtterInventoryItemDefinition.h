// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "OtterInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UOtterInventoryItemInstance;
struct FFrame;

//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OTTERGAME_API UOtterInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UOtterInventoryItemInstance* Instance) const {}
};

//////////////////////////////////////////////////////////////////////

/**
 * UOtterInventoryItemDefinition
 */
UCLASS(Blueprintable, Const, Abstract)
class UOtterInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UOtterInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UOtterInventoryItemFragment>> Fragments;

public:
	const UOtterInventoryItemFragment* FindFragmentByClass(TSubclassOf<UOtterInventoryItemFragment> FragmentClass) const;
};

//@TODO: Make into a subsystem instead?
UCLASS()
class UOtterInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UOtterInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, TSubclassOf<UOtterInventoryItemFragment> FragmentClass);
};
