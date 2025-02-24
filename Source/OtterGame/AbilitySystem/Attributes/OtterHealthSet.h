// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "OtterAttributeSet.h"
#include "NativeGameplayTags.h"

#include "OtterHealthSet.generated.h"

class UObject;
struct FFrame;

OTTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
OTTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
OTTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
OTTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
OTTERGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Otter_Damage_Message);

struct FGameplayEffectModCallbackData;


/**
 * UOtterHealthSet
 *
 *	Class that defines attributes that are necessary for taking damage.
 *	Attribute examples include: health, shields, and resistances.
 */
UCLASS(BlueprintType)
class OTTERGAME_API UOtterHealthSet : public UOtterAttributeSet
{
	GENERATED_BODY()

public:

	UOtterHealthSet();

	ATTRIBUTE_ACCESSORS(UOtterHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UOtterHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UOtterHealthSet, Healing);
	ATTRIBUTE_ACCESSORS(UOtterHealthSet, Damage);

	// Delegate when health changes due to damage/healing, some information may be missing on the client
	mutable FOtterAttributeEvent OnHealthChanged;

	// Delegate when max health changes
	mutable FOtterAttributeEvent OnMaxHealthChanged;

	// Delegate to broadcast when the health attribute reaches zero
	mutable FOtterAttributeEvent OnOutOfHealth;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Otter|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Otter|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Store the health before any changes 
	float MaxHealthBeforeAttributeChange;
	float HealthBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category="Otter|Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category="Otter|Health", Meta=(HideFromModifiers, AllowPrivateAccess=true))
	FGameplayAttributeData Damage;
};
