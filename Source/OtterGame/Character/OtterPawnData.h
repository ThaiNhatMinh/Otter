// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "OtterPawnData.generated.h"

class APawn;
class UOtterAbilitySet;
class UOtterAbilityTagRelationshipMapping;
class UOtterCameraMode;
class UOtterInputConfig;
class UObject;


/**
 * UOtterPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Otter Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class OTTERGAME_API UOtterPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UOtterPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from AOtterPawn or AOtterCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|Abilities")
	TArray<TObjectPtr<UOtterAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|Abilities")
	TObjectPtr<UOtterAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|Input")
	TObjectPtr<UOtterInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|Camera")
	TSubclassOf<UOtterCameraMode> DefaultCameraMode;
};
