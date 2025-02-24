// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterWeaponDebugSettings.h"
#include "Misc/App.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterWeaponDebugSettings)

UOtterWeaponDebugSettings::UOtterWeaponDebugSettings()
{
}

FName UOtterWeaponDebugSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

