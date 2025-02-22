// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameplayAbility_Jump.h"

#include "AbilitySystem/Abilities/OtterGameplayAbility.h"
#include "Character/OtterCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameplayAbility_Jump)

struct FGameplayTagContainer;


UOtterGameplayAbility_Jump::UOtterGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UOtterGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AOtterCharacter* OtterCharacter = Cast<AOtterCharacter>(ActorInfo->AvatarActor.Get());
	if (!OtterCharacter || !OtterCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UOtterGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOtterGameplayAbility_Jump::CharacterJumpStart()
{
	if (AOtterCharacter* OtterCharacter = GetOtterCharacterFromActorInfo())
	{
		if (OtterCharacter->IsLocallyControlled() && !OtterCharacter->bPressedJump)
		{
			OtterCharacter->UnCrouch();
			OtterCharacter->Jump();
		}
	}
}

void UOtterGameplayAbility_Jump::CharacterJumpStop()
{
	if (AOtterCharacter* OtterCharacter = GetOtterCharacterFromActorInfo())
	{
		if (OtterCharacter->IsLocallyControlled() && OtterCharacter->bPressedJump)
		{
			OtterCharacter->StopJumping();
		}
	}
}

