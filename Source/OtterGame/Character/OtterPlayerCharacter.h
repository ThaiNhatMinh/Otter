// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/OtterCharacter.h"

#include "OtterPlayerCharacter.generated.h"

class AActor;
class AController;
class AOtterPlayerController;
class AOtterPlayerState;
class FLifetimeProperty;
class UAbilitySystemComponent;
class UInputComponent;
class UOtterAbilitySystemComponent;
class UOtterCameraComponent;
class UOtterHealthComponent;
class UOtterPawnExtensionComponent;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;

/**
 * AOtterPlayerCharacter
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class OTTERGAME_API AOtterPlayerCharacter : public AOtterCharacter
{
	GENERATED_BODY()

public:

	AOtterPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Otter|Character")
	AOtterPlayerController* GetOtterPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|Character")
	AOtterPlayerState* GetOtterPlayerState() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//~AActor interface
	virtual void Reset() override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

protected:

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Otter|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOtterPawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Otter|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOtterHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Otter|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOtterCameraComponent> CameraComponent;

protected:
	// Called to determine what happens to the team ID when possession ends
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}

private:
};
