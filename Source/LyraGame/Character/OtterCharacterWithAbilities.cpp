// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/OtterCombatSet.h"
#include "AbilitySystem/Attributes/OtterHealthSet.h"
#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCharacterWithAbilities)

AOtterCharacterWithAbilities::AOtterCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UOtterAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UOtterHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UOtterCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void AOtterCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AOtterCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

