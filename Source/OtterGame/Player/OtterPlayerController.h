// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/OtterCameraAssistInterface.h"
#include "CommonPlayerController.h"

#include "OtterPlayerController.generated.h"

struct FGenericTeamId;

class AOtterHUD;
class AOtterPlayerState;
class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class UOtterAbilitySystemComponent;
class UOtterSettingsShared;
class UObject;
class UPlayer;
struct FFrame;

/**
 * AOtterPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class OTTERGAME_API AOtterPlayerController : public ACommonPlayerController, public IOtterCameraAssistInterface
{
	GENERATED_BODY()

public:

	AOtterPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerController")
	AOtterPlayerState* GetOtterPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerController")
	UOtterAbilitySystemComponent* GetOtterAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerController")
	AOtterHUD* GetOtterHUD() const;

	// Call from game state logic to start recording an automatic client replay if ShouldRecordClientReplay returns true
	UFUNCTION(BlueprintCallable, Category = "Otter|PlayerController")
	bool TryToRecordClientReplay();

	// Call to see if we should record a replay, subclasses could change this
	virtual bool ShouldRecordClientReplay();

	// Run a cheat command on the server.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheat(const FString& Msg);

	// Run a cheat command on the server for all players.
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCheatAll(const FString& Msg);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	//~IOtterCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IOtterCameraAssistInterface interface
private:
	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;

protected:
	// Called when the player state is set or cleared
	virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:

	//~APlayerController interface

	//~End of APlayerController interface

	void OnSettingsChanged(UOtterSettingsShared* Settings);
	
	bool bHideViewTargetPawnNextFrame = false;
};


// A player controller used for replay capture and playback
UCLASS()
class AOtterReplayPlayerController : public AOtterPlayerController
{
	GENERATED_BODY()

	virtual void Tick(float DeltaSeconds) override;
	virtual void SmoothTargetViewRotation(APawn* TargetPawn, float DeltaSeconds) override;
	virtual bool ShouldRecordClientReplay() override;

	// Callback for when the game state's RecorderPlayerState gets replicated during replay playback
	void RecorderPlayerStateUpdated(APlayerState* NewRecorderPlayerState);

	// Callback for when the followed player state changes pawn
	UFUNCTION()
	void OnPlayerStatePawnSet(APlayerState* ChangedPlayerState, APawn* NewPlayerPawn, APawn* OldPlayerPawn);

	// The player state we are currently following */
	UPROPERTY(Transient)
	TObjectPtr<APlayerState> FollowedPlayerState;
};
