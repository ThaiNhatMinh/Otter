// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterEditorEngine.h"

#include "Development/OtterDeveloperSettings.h"
#include "Development/OtterPlatformEmulationSettings.h"
#include "Engine/GameInstance.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GameModes/OtterWorldSettings.h"
#include "Settings/ContentBrowserSettings.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Widgets/Notifications/SNotificationList.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterEditorEngine)

class IEngineLoop;

#define LOCTEXT_NAMESPACE "OtterEditor"

UOtterEditorEngine::UOtterEditorEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOtterEditorEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

void UOtterEditorEngine::Start()
{
	Super::Start();
}

void UOtterEditorEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);
	
	FirstTickSetup();
}

void UOtterEditorEngine::FirstTickSetup()
{
	if (bFirstTickSetup)
	{
		return;
	}

	bFirstTickSetup = true;

	// Force show plugin content on load.
	GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders(true);

}

FGameInstancePIEResult UOtterEditorEngine::PreCreatePIEInstances(const bool bAnyBlueprintErrors, const bool bStartInSpectatorMode, const float PIEStartTime, const bool bSupportsOnlinePIE, int32& InNumOnlinePIEInstances)
{
	if (const AOtterWorldSettings* OtterWorldSettings = Cast<AOtterWorldSettings>(EditorWorld->GetWorldSettings()))
	{
		if (OtterWorldSettings->ForceStandaloneNetMode)
		{
			EPlayNetMode OutPlayNetMode;
			PlaySessionRequest->EditorPlaySettings->GetPlayNetMode(OutPlayNetMode);
			if (OutPlayNetMode != PIE_Standalone)
			{
				PlaySessionRequest->EditorPlaySettings->SetPlayNetMode(PIE_Standalone);

				FNotificationInfo Info(LOCTEXT("ForcingStandaloneForFrontend", "Forcing NetMode: Standalone for the Frontend"));
				Info.ExpireDuration = 2.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
			}
		}
	}

	//@TODO: Should add delegates that a *non-editor* module could bind to for PIE start/stop instead of poking directly
	GetDefault<UOtterDeveloperSettings>()->OnPlayInEditorStarted();
	GetDefault<UOtterPlatformEmulationSettings>()->OnPlayInEditorStarted();

	//
	FGameInstancePIEResult Result = Super::PreCreatePIEServerInstance(bAnyBlueprintErrors, bStartInSpectatorMode, PIEStartTime, bSupportsOnlinePIE, InNumOnlinePIEInstances);

	return Result;
}

#undef LOCTEXT_NAMESPACE
