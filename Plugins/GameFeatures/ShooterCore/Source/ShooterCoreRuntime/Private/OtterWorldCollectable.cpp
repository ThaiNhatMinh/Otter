// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterWorldCollectable.h"

#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterWorldCollectable)

struct FInteractionQuery;

AOtterWorldCollectable::AOtterWorldCollectable()
{
}

void AOtterWorldCollectable::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}

FInventoryPickup AOtterWorldCollectable::GetPickupInventory() const
{
	return StaticInventory;
}
