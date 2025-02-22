// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAttributeSet.h"

#include "AbilitySystem/OtterAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterAttributeSet)

class UWorld;


UOtterAttributeSet::UOtterAttributeSet()
{
}

UWorld* UOtterAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UOtterAbilitySystemComponent* UOtterAttributeSet::GetOtterAbilitySystemComponent() const
{
	return Cast<UOtterAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

