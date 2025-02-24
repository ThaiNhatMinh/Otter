// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/OtterLocalPlayer.h"

#include "AudioMixerBlueprintLibrary.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Settings/OtterSettingsLocal.h"
#include "Settings/OtterSettingsShared.h"
#include "CommonUserSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterLocalPlayer)

class UObject;

UOtterLocalPlayer::UOtterLocalPlayer()
{
}

void UOtterLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UOtterSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UOtterLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

void UOtterLocalPlayer::SwitchController(class APlayerController* PC)
{
	Super::SwitchController(PC);

	OnPlayerControllerChanged(PlayerController);
}

bool UOtterLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	OnPlayerControllerChanged(PlayerController);

	return bResult;
}

void UOtterLocalPlayer::InitOnlineSession()
{
	OnPlayerControllerChanged(PlayerController);

	Super::InitOnlineSession();
}

void UOtterLocalPlayer::OnPlayerControllerChanged(APlayerController* NewController)
{
}

UOtterSettingsLocal* UOtterLocalPlayer::GetLocalSettings() const
{
	return UOtterSettingsLocal::Get();
}

UOtterSettingsShared* UOtterLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
	{
		// On PC it's okay to use the sync load because it only checks the disk
		// This could use a platform tag to check for proper save support instead
		bool bCanLoadBeforeLogin = PLATFORM_DESKTOP;
		
		if (bCanLoadBeforeLogin)
		{
			SharedSettings = UOtterSettingsShared::LoadOrCreateSettings(this);
		}
		else
		{
			// We need to wait for user login to get the real settings so return temp ones
			SharedSettings = UOtterSettingsShared::CreateTemporarySettings(this);
		}
	}

	return SharedSettings;
}

void UOtterLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	FUniqueNetIdRepl CurrentNetId = GetCachedUniqueNetId();
	if (!bForceLoad && SharedSettings && CurrentNetId == NetIdForSharedSettings)
	{
		// Already loaded once, don't reload
		return;
	}

	ensure(UOtterSettingsShared::AsyncLoadOrCreateSettings(this, UOtterSettingsShared::FOnSettingsLoadedEvent::CreateUObject(this, &UOtterLocalPlayer::OnSharedSettingsLoaded)));
}

void UOtterLocalPlayer::OnSharedSettingsLoaded(UOtterSettingsShared* LoadedOrCreatedSettings)
{
	// The settings are applied before it gets here
	if (ensure(LoadedOrCreatedSettings))
	{
		// This will replace the temporary or previously loaded object which will GC out normally
		SharedSettings = LoadedOrCreatedSettings;

		NetIdForSharedSettings = GetCachedUniqueNetId();
	}
}

void UOtterLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UOtterLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}


