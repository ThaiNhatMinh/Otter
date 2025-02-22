// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPerformanceSettings.h"

#include "Engine/PlatformSettingsManager.h"
#include "Misc/EnumRange.h"
#include "Performance/OtterPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPerformanceSettings)

//////////////////////////////////////////////////////////////////////

UOtterPlatformSpecificRenderingSettings::UOtterPlatformSpecificRenderingSettings()
{
	MobileFrameRateLimits.Append({ 20, 30, 45, 60, 90, 120 });
}

const UOtterPlatformSpecificRenderingSettings* UOtterPlatformSpecificRenderingSettings::Get()
{
	UOtterPlatformSpecificRenderingSettings* Result = UPlatformSettingsManager::Get().GetSettingsForPlatform<ThisClass>();
	check(Result);
	return Result;
}

//////////////////////////////////////////////////////////////////////

UOtterPerformanceSettings::UOtterPerformanceSettings()
{
	PerPlatformSettings.Initialize(UOtterPlatformSpecificRenderingSettings::StaticClass());

	CategoryName = TEXT("Game");

	DesktopFrameRateLimits.Append({ 30, 60, 120, 144, 160, 165, 180, 200, 240, 360 });

	// Default to all stats are allowed
	FOtterPerformanceStatGroup& StatGroup = UserFacingPerformanceStats.AddDefaulted_GetRef();
	for (EOtterDisplayablePerformanceStat PerfStat : TEnumRange<EOtterDisplayablePerformanceStat>())
	{
		StatGroup.AllowedStats.Add(PerfStat);
	}
}

