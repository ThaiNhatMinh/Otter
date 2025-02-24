// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularAIController.h"
#include "Teams/OtterTeamAgentInterface.h"

#include "OtterPlayerBotController.generated.h"

namespace ETeamAttitude { enum Type : int; }
struct FGenericTeamId;

class APlayerState;
class UAIPerceptionComponent;
class UObject;
struct FFrame;

/**
 * AOtterPlayerBotController
 *
 *	The controller class used by player bots in this project.
 */
UCLASS(Blueprintable)
class AOtterPlayerBotController : public AModularAIController
{
	GENERATED_BODY()

public:
	AOtterPlayerBotController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Attempts to restart this controller (e.g., to respawn it)
	void ServerRestartController();

	//Update Team Attitude for the AI
	UFUNCTION(BlueprintCallable, Category = "Otter AI Player Controller")
	void UpdateTeamAttitude(UAIPerceptionComponent* AIPerception);

	virtual void OnUnPossess() override;


private:
	UFUNCTION()
	void OnPlayerStateChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:	
	//~AController interface
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

private:
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;
};
