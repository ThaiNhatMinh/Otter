// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettingsBackedByCVars.h"

#include "OtterWeaponDebugSettings.generated.h"

class UObject;

/**
 * Developer debugging settings for weapons
 */
UCLASS(config=EditorPerProjectUserSettings)
class UOtterWeaponDebugSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UOtterWeaponDebugSettings();

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface

public:
	// Should we do debug drawing for bullet traces (if above zero, sets how long (in seconds)
	// UPROPERTY(config, EditAnywhere, Category=General, meta=(ConsoleVariable="lyra.Weapon.DrawBulletTraceDuration", ForceUnits=s))
	// float DrawBulletTraceDuration;
};
