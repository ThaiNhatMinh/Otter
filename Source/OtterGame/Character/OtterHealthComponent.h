// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameFrameworkComponent.h"

#include "OtterHealthComponent.generated.h"

class UOtterHealthComponent;

class UOtterAbilitySystemComponent;
class UOtterHealthSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOtterHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOtterHealth_AttributeChanged, UOtterHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EOtterDeathState
 *
 *	Defines current state of death.
 */
UENUM(BlueprintType)
enum class EOtterDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * UOtterHealthComponent
 *
 *	An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class OTTERGAME_API UOtterHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	UOtterHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Otter|Health")
	static UOtterHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UOtterHealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	void InitializeWithAbilitySystem(UOtterAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|Health")
	EOtterDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Otter|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EOtterDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FOtterHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FOtterHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FOtterHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FOtterHealth_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	virtual void OnRep_DeathState(EOtterDeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UOtterAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UOtterHealthSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EOtterDeathState DeathState;
};
