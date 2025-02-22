// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "ShooterTestsInputTestHelper.h"

class AOtterCharacter;
class USkeletalMeshComponent;

/// Class which consolidates the Otter Actor information that is shared amongst tests.
class FShooterTestsActorTestHelper
{
public:
	/**
	* Construct the Actor Test Helper object
	* 
	* @param Pawn - Pointer to a Pawn
	*/
	explicit FShooterTestsActorTestHelper(APawn* Pawn);

	/**
	* Checks to see if the current actor is fully spawned in the level and ready to be used.
	* 
	* @return true is the player is spawned and usable, otherwise false
	*/
	bool IsPawnFullySpawned();

	/**
	* Gets the Otter character which was associated with the Pawn used during the construction of the object.
	*
	* @return constant pointer to the OtterCharacter
	*/
	const AOtterCharacter* GetOtterCharacter() const { return OtterCharacter; }
	
	/**
	* Gets the skeletal mesh component of the associated Otter Character.
	*
	* @return pointer to the SkeletalMeshComponent
	*/
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }

private:
	/** Reference to our player in the level. */
	AOtterCharacter* OtterCharacter{ nullptr };

	/** Reference to the player's skeletal mesh component. */
	USkeletalMeshComponent* SkeletalMeshComponent;

	/** Reference to the player's ability system component. */
	UOtterAbilitySystemComponent* AbilitySystemComponent{ nullptr };

	/** Reference to the player's spawning gameplay effect. */
	FGameplayTag GameplayCueCharacterSpawnTag;
};

/**
 * Inherited from FShooterTestsActorTestHelper, adds FShooterTestsPawnTestActions to be used for interacting with the Otter player and triggering animations.
 *
 * @see FShooterTestsActorTestHelper
 */
class FShooterTestsActorInputTestHelper : public FShooterTestsActorTestHelper
{
public:
	/** Defines the available input actions that can be performed. */
	enum class InputActionType : uint8_t
	{
		Crouch,
		Melee,
		Jump,
		MoveForward,
		MoveBackward,
		StrafeLeft,
		StrafeRight,
	};

	explicit FShooterTestsActorInputTestHelper(APawn* Pawn);

	/** 
	* Simulates input triggers on the Otter character.
	* 
	* @param Type - InputActionType used to specify which input to perform.
	*/
	void PerformInput(InputActionType Type);

	/**
	* Stops all actively running inputs.
	*/
	void StopAllInput();

private:
	/** Object which handles interfacing with the Enhanced Input System to perform input actions. */
	TUniquePtr<FShooterTestsPawnTestActions> PawnActions;
};