// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/OtterCharacter.h"

#include "OtterCharacterWithAbilities.generated.h"

class UAbilitySystemComponent;
class UOtterAbilitySystemComponent;
class UObject;

// AOtterCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(Blueprintable)
class OTTERGAME_API AOtterCharacterWithAbilities : public AOtterCharacter
{
	GENERATED_BODY()

public:
	AOtterCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Otter|PlayerState")
	TObjectPtr<UOtterAbilitySystemComponent> AbilitySystemComponent;
};
