// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterCombatSet.h"

#include "AbilitySystem/Attributes/OtterAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCombatSet)

class FLifetimeProperty;


UOtterCombatSet::UOtterCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UOtterCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UOtterCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOtterCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UOtterCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOtterCombatSet, BaseDamage, OldValue);
}

void UOtterCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOtterCombatSet, BaseHeal, OldValue);
}

