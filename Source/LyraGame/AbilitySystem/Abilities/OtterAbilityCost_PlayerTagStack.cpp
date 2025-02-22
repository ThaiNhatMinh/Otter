// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "OtterGameplayAbility.h"
#include "Player/OtterPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterAbilityCost_PlayerTagStack)

UOtterAbilityCost_PlayerTagStack::UOtterAbilityCost_PlayerTagStack()
{
	Quantity.SetValue(1.0f);
}

bool UOtterAbilityCost_PlayerTagStack::CheckCost(const UOtterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (AController* PC = Ability->GetControllerFromActorInfo())
	{
		if (AOtterPlayerState* PS = Cast<AOtterPlayerState>(PC->PlayerState))
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

			return PS->GetStatTagStackCount(Tag) >= NumStacks;
		}
	}
	return false;
}

void UOtterAbilityCost_PlayerTagStack::ApplyCost(const UOtterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (AController* PC = Ability->GetControllerFromActorInfo())
		{
			if (AOtterPlayerState* PS = Cast<AOtterPlayerState>(PC->PlayerState))
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

				PS->RemoveStatTagStack(Tag, NumStacks);
			}
		}
	}
}

