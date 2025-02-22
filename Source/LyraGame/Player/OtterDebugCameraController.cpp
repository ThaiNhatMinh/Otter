// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterDebugCameraController.h"
#include "OtterCheatManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterDebugCameraController)


AOtterDebugCameraController::AOtterDebugCameraController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Use the same cheat class as OtterPlayerController to allow toggling the debug camera through cheats.
	CheatClass = UOtterCheatManager::StaticClass();
}

void AOtterDebugCameraController::AddCheats(bool bForce)
{
	// Mirrors OtterPlayerController's AddCheats() to avoid the player becoming stuck in the debug camera.
#if USING_CHEAT_MANAGER
	Super::AddCheats(true);
#else //#if USING_CHEAT_MANAGER
	Super::AddCheats(bForce);
#endif // #else //#if USING_CHEAT_MANAGER
}

