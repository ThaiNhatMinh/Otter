// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAbilitySystemGlobals.h"

#include "OtterGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterAbilitySystemGlobals)

struct FGameplayEffectContext;

UOtterAbilitySystemGlobals::UOtterAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UOtterAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FOtterGameplayEffectContext();
}

