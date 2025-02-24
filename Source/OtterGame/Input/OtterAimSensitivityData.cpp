// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterAimSensitivityData.h"

#include "Settings/OtterSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterAimSensitivityData)

UOtterAimSensitivityData::UOtterAimSensitivityData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SensitivityMap =
	{
		{ EOtterGamepadSensitivity::Slow,			0.5f },
		{ EOtterGamepadSensitivity::SlowPlus,		0.75f },
		{ EOtterGamepadSensitivity::SlowPlusPlus,	0.9f },
		{ EOtterGamepadSensitivity::Normal,		1.0f },
		{ EOtterGamepadSensitivity::NormalPlus,	1.1f },
		{ EOtterGamepadSensitivity::NormalPlusPlus,1.25f },
		{ EOtterGamepadSensitivity::Fast,			1.5f },
		{ EOtterGamepadSensitivity::FastPlus,		1.75f },
		{ EOtterGamepadSensitivity::FastPlusPlus,	2.0f },
		{ EOtterGamepadSensitivity::Insane,		2.5f },
	};
}

const float UOtterAimSensitivityData::SensitivtyEnumToFloat(const EOtterGamepadSensitivity InSensitivity) const
{
	if (const float* Sens = SensitivityMap.Find(InSensitivity))
	{
		return *Sens;
	}

	return 1.0f;
}

