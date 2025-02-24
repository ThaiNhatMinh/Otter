// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameplayEffectContext.h"

#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameplayEffectContext)

class FArchive;

FOtterGameplayEffectContext* FOtterGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FOtterGameplayEffectContext::StaticStruct()))
	{
		return (FOtterGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FOtterGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);
	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FOtterGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(OtterGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

const UPhysicalMaterial* FOtterGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

