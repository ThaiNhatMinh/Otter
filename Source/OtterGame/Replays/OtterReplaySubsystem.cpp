// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterReplaySubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"
#include "Internationalization/Text.h"
#include "Misc/DateTime.h"
#include "CommonUISettings.h"
#include "ICommonUIModule.h"
#include "OtterLogChannels.h"
#include "Player/OtterLocalPlayer.h"
#include "Settings/OtterSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterReplaySubsystem)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Platform_Trait_ReplaySupport, "Platform.Trait.ReplaySupport");

UOtterReplaySubsystem::UOtterReplaySubsystem()
{
}

bool UOtterReplaySubsystem::DoesPlatformSupportReplays()
{
	if (ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GetPlatformSupportTraitTag()))
	{
		return true;
	}
	return false;
}

FGameplayTag UOtterReplaySubsystem::GetPlatformSupportTraitTag()
{
	return TAG_Platform_Trait_ReplaySupport.GetTag();
}

void UOtterReplaySubsystem::PlayReplay(UOtterReplayListEntry* Replay)
{
	if (Replay != nullptr)
	{
		FString DemoName = Replay->StreamInfo.Name;
		GetGameInstance()->PlayReplay(DemoName);
	}
}

void UOtterReplaySubsystem::RecordClientReplay(APlayerController* PlayerController)
{
	if (ensure(DoesPlatformSupportReplays() && PlayerController))
	{
		FText FriendlyNameText = FText::Format(NSLOCTEXT("Otter", "OtterReplayName_Format", "Client Replay {0}"), FText::AsDateTime(FDateTime::UtcNow(), EDateTimeStyle::Short, EDateTimeStyle::Short));
		GetGameInstance()->StartRecordingReplay(FString(), FriendlyNameText.ToString());

		if (UOtterLocalPlayer* OtterLocalPlayer = Cast<UOtterLocalPlayer>(PlayerController->GetLocalPlayer()))
		{
			// Start a cleanup of existing saved streams
			int32 NumToKeep = OtterLocalPlayer->GetLocalSettings()->GetNumberOfReplaysToKeep();
			CleanupLocalReplays(OtterLocalPlayer, NumToKeep);
		}
	}
}

void UOtterReplaySubsystem::CleanupLocalReplays(ULocalPlayer* LocalPlayer, int32 NumReplaysToKeep)
{
	// TODO this was only tested with the generic file streamer and may not fully work with the save game streamer
	// This only handles one delete at a time, and will loop until it gets an error or goes below NumReplaysToKeep
	// It does it this way because each delete may involve a server or save game query that invalidates the replay list
	if (LocalPlayer != nullptr && LocalPlayerDeletingReplays == nullptr && NumReplaysToKeep != 0)
	{
		LocalPlayerDeletingReplays = LocalPlayer;
		DeletingReplaysNumberToKeep = NumReplaysToKeep;

		CurrentReplayStreamer = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();
		if (CurrentReplayStreamer.IsValid())
		{
			// Use the default version to get old version replays as well
			FNetworkReplayVersion EnumerateStreamsVersion;

			CurrentReplayStreamer->EnumerateStreams(EnumerateStreamsVersion, LocalPlayer->GetPlatformUserIndex(), FString(), TArray<FString>(), FEnumerateStreamsCallback::CreateUObject(this, &ThisClass::OnEnumerateStreamsCompleteForDelete));
		}
	}
}

void UOtterReplaySubsystem::OnEnumerateStreamsCompleteForDelete(const FEnumerateStreamsResult& Result)
{
	if (!CurrentReplayStreamer.IsValid() || !IsValid(LocalPlayerDeletingReplays))
	{
		// Lost context, don't do anything
		return;
	}

	TArray<FNetworkReplayStreamInfo> StreamsToDelete;
	for (const FNetworkReplayStreamInfo& StreamInfo : Result.FoundStreams)
	{
		// Never delete keep streams
		if (!StreamInfo.bShouldKeep)
		{
			StreamsToDelete.Add(StreamInfo);
		}
	}

	// Sort by date
	Algo::SortBy(StreamsToDelete, [](const FNetworkReplayStreamInfo& Data) { return Data.Timestamp.GetTicks(); }, TGreater<>());

	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		if (DemoDriver->IsRecording())
		{
			// If we're recording, the live stream may or may not show up in the query which affects the keep count
			// Add a fake live stream if the active one is missing from the results
			if (StreamsToDelete.Num() > 0 && !StreamsToDelete[0].bIsLive)
			{
				StreamsToDelete.Insert(FNetworkReplayStreamInfo(), 0);
			}
		}
	}

	if (StreamsToDelete.Num() > DeletingReplaysNumberToKeep)
	{
		// Delete the first replay above the limit, if successful it won't be in the loop during the next loop
		// If unsuccessful, it will stop looping
		FString ReplayName = StreamsToDelete[DeletingReplaysNumberToKeep].Name;
		UE_LOG(LogOtter, Log, TEXT("OtterReplaySubsystem asked to delete replay %s"), *ReplayName);
		CurrentReplayStreamer->DeleteFinishedStream(ReplayName, LocalPlayerDeletingReplays->GetPlatformUserIndex(), FDeleteFinishedStreamCallback::CreateUObject(this, &ThisClass::OnDeleteReplay));
	}
	else
	{
		// We're below the limit so stop iterating
		CurrentReplayStreamer = nullptr;
		LocalPlayerDeletingReplays = nullptr;
		DeletingReplaysNumberToKeep = 0;
	}
}

void UOtterReplaySubsystem::OnDeleteReplay(const FDeleteFinishedStreamResult& DeleteResult)
{
	if (!CurrentReplayStreamer.IsValid() || !IsValid(LocalPlayerDeletingReplays))
	{
		// Lost context, don't do anything
		return;
	}

	if (DeleteResult.WasSuccessful())
	{
		// Enumerate list again to see if we're under the limit yet
		FNetworkReplayVersion EnumerateStreamsVersion;

		CurrentReplayStreamer->EnumerateStreams(EnumerateStreamsVersion, LocalPlayerDeletingReplays->GetPlatformUserIndex(), FString(), TArray<FString>(), FEnumerateStreamsCallback::CreateUObject(this, &ThisClass::OnEnumerateStreamsCompleteForDelete));
	}
	else
	{
		// Failed, stop trying to delete anything else
		// TODO properly integrate with platform-specific error reporting
		UE_LOG(LogOtter, Warning, TEXT("Failed to delete replay with error %d!"), (int32)DeleteResult.Result);

		CurrentReplayStreamer = nullptr;
		LocalPlayerDeletingReplays = nullptr;
		DeletingReplaysNumberToKeep = 0;
	}
}

void UOtterReplaySubsystem::SeekInActiveReplay(float TimeInSeconds)
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		DemoDriver->GotoTimeInSeconds(TimeInSeconds);
	}
}

float UOtterReplaySubsystem::GetReplayLengthInSeconds() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoTotalTime();
	}
	return 0.0f;
}

float UOtterReplaySubsystem::GetReplayCurrentTime() const
{
	if (UDemoNetDriver* DemoDriver = GetDemoDriver())
	{
		return DemoDriver->GetDemoCurrentTime();
	}
	return 0.0f;
}

UDemoNetDriver* UOtterReplaySubsystem::GetDemoDriver() const
{
	if (UWorld* World = GetGameInstance()->GetWorld())
	{
		return World->GetDemoNetDriver();
	}
	return nullptr;
}



