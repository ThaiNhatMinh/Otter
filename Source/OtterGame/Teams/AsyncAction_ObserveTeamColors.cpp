// Copyright Epic Games, Inc. All Rights Reserved.

#include "Teams/AsyncAction_ObserveTeamColors.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Teams/OtterTeamAgentInterface.h"
#include "Teams/OtterTeamStatics.h"
#include "Teams/OtterTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ObserveTeamColors)

UAsyncAction_ObserveTeamColors::UAsyncAction_ObserveTeamColors(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAsyncAction_ObserveTeamColors* UAsyncAction_ObserveTeamColors::ObserveTeamColors(UObject* TeamAgent)
{
	UAsyncAction_ObserveTeamColors* Action = nullptr;

	if (TeamAgent != nullptr)
	{
		Action = NewObject<UAsyncAction_ObserveTeamColors>();
		Action->TeamInterfacePtr = TWeakInterfacePtr<IOtterTeamAgentInterface>(TeamAgent);
		Action->TeamInterfaceObj = TeamAgent;
		Action->RegisterWithGameInstance(TeamAgent);
	}

	return Action;
}

void UAsyncAction_ObserveTeamColors::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();

	// If we're being canceled we need to unhook everything we might have tried listening to.
	if (IOtterTeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		TeamInterface->GetOnTeamIndexChangedDelegate().RemoveAll(this);
	}
}

void UAsyncAction_ObserveTeamColors::Activate()
{
	bool bCouldSucceed = false;
	int32 CurrentTeamIndex = INDEX_NONE;
	UOtterTeamDisplayAsset* CurrentDisplayAsset = nullptr;

	if (IOtterTeamAgentInterface* TeamInterface = TeamInterfacePtr.Get())
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(TeamInterfaceObj.Get(), EGetWorldErrorMode::LogAndReturnNull))
		{
			// Get current team info
			CurrentTeamIndex = GenericTeamIdToInteger(TeamInterface->GetGenericTeamId());
			CurrentDisplayAsset = UOtterTeamStatics::GetTeamDisplayAsset(World, CurrentTeamIndex);

			// Listen for team changes in the future
			TeamInterface->GetOnTeamIndexChangedDelegate().AddUObject(this, &ThisClass::OnWatchedAgentChangedTeam);

			bCouldSucceed = true;
		}
	}

	// Broadcast once so users get the current state
	BroadcastChange(CurrentTeamIndex, CurrentDisplayAsset);

	// We weren't able to bind to a delegate so we'll never get any additional updates
	if (!bCouldSucceed)
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_ObserveTeamColors::BroadcastChange(int32 NewTeam, const UOtterTeamDisplayAsset* DisplayAsset)
{
	UWorld* World = GEngine->GetWorldFromContextObject(TeamInterfaceObj.Get(), EGetWorldErrorMode::LogAndReturnNull);
	UOtterTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<UOtterTeamSubsystem>(World);

	const bool bTeamChanged = (LastBroadcastTeamId != NewTeam);

	// Stop listening on the old team
	if ((TeamSubsystem != nullptr) && bTeamChanged && (LastBroadcastTeamId != INDEX_NONE))
	{
		TeamSubsystem->GetTeamDisplayAssetChangedDelegate(LastBroadcastTeamId).RemoveAll(this);
	}

	// Broadcast
	LastBroadcastTeamId = NewTeam;
	OnTeamChanged.Broadcast(NewTeam != INDEX_NONE, NewTeam, DisplayAsset);

	// Start listening on the new team
	if ((TeamSubsystem != nullptr) && bTeamChanged && (NewTeam != INDEX_NONE))
	{
		TeamSubsystem->GetTeamDisplayAssetChangedDelegate(NewTeam).AddDynamic(this, &ThisClass::OnDisplayAssetChanged);
	}
}

void UAsyncAction_ObserveTeamColors::OnWatchedAgentChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	UOtterTeamDisplayAsset* DisplayAsset = UOtterTeamStatics::GetTeamDisplayAsset(TeamAgent, NewTeam);
	BroadcastChange(NewTeam, DisplayAsset);
}

void UAsyncAction_ObserveTeamColors::OnDisplayAssetChanged(const UOtterTeamDisplayAsset* DisplayAsset)
{
	BroadcastChange(LastBroadcastTeamId, DisplayAsset);
}

