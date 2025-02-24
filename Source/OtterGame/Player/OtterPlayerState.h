// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "System/GameplayTagStack.h"
#include "Teams/OtterTeamAgentInterface.h"

#include "OtterPlayerState.generated.h"

struct FOtterVerbMessage;

class AController;
class AOtterPlayerController;
class APlayerState;
class FName;
class UAbilitySystemComponent;
class UOtterAbilitySystemComponent;
class UOtterExperienceDefinition;
class UOtterPawnData;
class UObject;
struct FFrame;
struct FGameplayTag;

/** Defines the types of client connected */
UENUM()
enum class EOtterPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * AOtterPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class OTTERGAME_API AOtterPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public IOtterTeamAgentInterface
{
	GENERATED_BODY()

public:
	AOtterPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerState")
	AOtterPlayerController* GetOtterPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerState")
	UOtterAbilitySystemComponent* GetOtterAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UOtterPawnData* InPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	virtual void Reset() override;
	virtual void ClientInitialize(AController* C) override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnDeactivated() override;
	virtual void OnReactivated() override;
	//~End of APlayerState interface

	//~IOtterTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~End of IOtterTeamAgentInterface interface

	static const FName NAME_OtterAbilityReady;

	void SetPlayerConnectionType(EOtterPlayerConnectionType NewType);
	EOtterPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Teams)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Teams)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Teams)
	bool HasStatTag(FGameplayTag Tag) const;

	// Send a message to just this player
	// (use only for client notifications like accolades, quest toasts, etc... that can handle being occasionally lost)
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "Otter|PlayerState")
	void ClientBroadcastMessage(const FOtterVerbMessage Message);

private:
	void OnExperienceLoaded(const UOtterExperienceDefinition* CurrentExperience);

protected:
	UFUNCTION()
	void OnRep_PawnData();
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UOtterPawnData> PawnData;

	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Otter|PlayerState")
	TObjectPtr<UOtterAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	EOtterPlayerConnectionType MyPlayerConnectionType;

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;
};
