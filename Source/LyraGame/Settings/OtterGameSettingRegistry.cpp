// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameSettingRegistry.h"

#include "GameSettingCollection.h"
#include "OtterSettingsLocal.h"
#include "OtterSettingsShared.h"
#include "Player/OtterLocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameSettingRegistry)

DEFINE_LOG_CATEGORY(LogOtterGameSettingRegistry);

#define LOCTEXT_NAMESPACE "Otter"

//--------------------------------------
// UOtterGameSettingRegistry
//--------------------------------------

UOtterGameSettingRegistry::UOtterGameSettingRegistry()
{
}

UOtterGameSettingRegistry* UOtterGameSettingRegistry::Get(UOtterLocalPlayer* InLocalPlayer)
{
	UOtterGameSettingRegistry* Registry = FindObject<UOtterGameSettingRegistry>(InLocalPlayer, TEXT("OtterGameSettingRegistry"), true);
	if (Registry == nullptr)
	{
		Registry = NewObject<UOtterGameSettingRegistry>(InLocalPlayer, TEXT("OtterGameSettingRegistry"));
		Registry->Initialize(InLocalPlayer);
	}

	return Registry;
}

bool UOtterGameSettingRegistry::IsFinishedInitializing() const
{
	if (Super::IsFinishedInitializing())
	{
		if (UOtterLocalPlayer* LocalPlayer = Cast<UOtterLocalPlayer>(OwningLocalPlayer))
		{
			if (LocalPlayer->GetSharedSettings() == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

void UOtterGameSettingRegistry::OnInitialize(ULocalPlayer* InLocalPlayer)
{
	UOtterLocalPlayer* OtterLocalPlayer = Cast<UOtterLocalPlayer>(InLocalPlayer);

	VideoSettings = InitializeVideoSettings(OtterLocalPlayer);
	InitializeVideoSettings_FrameRates(VideoSettings, OtterLocalPlayer);
	RegisterSetting(VideoSettings);

	AudioSettings = InitializeAudioSettings(OtterLocalPlayer);
	RegisterSetting(AudioSettings);

	GameplaySettings = InitializeGameplaySettings(OtterLocalPlayer);
	RegisterSetting(GameplaySettings);

	MouseAndKeyboardSettings = InitializeMouseAndKeyboardSettings(OtterLocalPlayer);
	RegisterSetting(MouseAndKeyboardSettings);

	GamepadSettings = InitializeGamepadSettings(OtterLocalPlayer);
	RegisterSetting(GamepadSettings);
}

void UOtterGameSettingRegistry::SaveChanges()
{
	Super::SaveChanges();
	
	if (UOtterLocalPlayer* LocalPlayer = Cast<UOtterLocalPlayer>(OwningLocalPlayer))
	{
		// Game user settings need to be applied to handle things like resolution, this saves indirectly
		LocalPlayer->GetLocalSettings()->ApplySettings(false);
		
		LocalPlayer->GetSharedSettings()->ApplySettings();
		LocalPlayer->GetSharedSettings()->SaveSettings();
	}
}

#undef LOCTEXT_NAMESPACE

