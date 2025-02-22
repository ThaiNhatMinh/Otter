// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterInventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterInventoryItemDefinition)

//////////////////////////////////////////////////////////////////////
// UOtterInventoryItemDefinition

UOtterInventoryItemDefinition::UOtterInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UOtterInventoryItemFragment* UOtterInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UOtterInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UOtterInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////
// UOtterInventoryItemDefinition

const UOtterInventoryItemFragment* UOtterInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, TSubclassOf<UOtterInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UOtterInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

