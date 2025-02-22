// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterSettingValueDiscrete_MobileFPSType.h"

#include "Performance/OtterPerformanceSettings.h"
#include "Settings/OtterSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterSettingValueDiscrete_MobileFPSType)

#define LOCTEXT_NAMESPACE "OtterSettings"

UOtterSettingValueDiscrete_MobileFPSType::UOtterSettingValueDiscrete_MobileFPSType()
{
}

void UOtterSettingValueDiscrete_MobileFPSType::OnInitialized()
{
	Super::OnInitialized();

	const UOtterPlatformSpecificRenderingSettings* PlatformSettings = UOtterPlatformSpecificRenderingSettings::Get();
	const UOtterSettingsLocal* UserSettings = UOtterSettingsLocal::Get();

	for (int32 TestLimit : PlatformSettings->MobileFrameRateLimits)
	{
		if (UOtterSettingsLocal::IsSupportedMobileFramePace(TestLimit))
		{
			FPSOptions.Add(TestLimit, MakeLimitString(TestLimit));
		}
	}

	const int32 FirstFrameRateWithQualityLimit = UserSettings->GetFirstFrameRateWithQualityLimit();
	if (FirstFrameRateWithQualityLimit > 0)
	{
		SetWarningRichText(FText::Format(LOCTEXT("MobileFPSType_Note", "<strong>Note: Changing the framerate setting to {0} or higher might lower your Quality Presets.</>"), MakeLimitString(FirstFrameRateWithQualityLimit)));
	}
}

int32 UOtterSettingValueDiscrete_MobileFPSType::GetDefaultFPS() const
{
	return UOtterSettingsLocal::GetDefaultMobileFrameRate();
}

FText UOtterSettingValueDiscrete_MobileFPSType::MakeLimitString(int32 Number)
{
	return FText::Format(LOCTEXT("MobileFrameRateOption", "{0} FPS"), FText::AsNumber(Number));
}

void UOtterSettingValueDiscrete_MobileFPSType::StoreInitial()
{
	InitialValue = GetValue();
}

void UOtterSettingValueDiscrete_MobileFPSType::ResetToDefault()
{
	SetValue(GetDefaultFPS(), EGameSettingChangeReason::ResetToDefault);
}

void UOtterSettingValueDiscrete_MobileFPSType::RestoreToInitial()
{
	SetValue(InitialValue, EGameSettingChangeReason::RestoreToInitial);
}

void UOtterSettingValueDiscrete_MobileFPSType::SetDiscreteOptionByIndex(int32 Index)
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);

	int32 NewMode = FPSOptionsModes.IsValidIndex(Index) ? FPSOptionsModes[Index] : GetDefaultFPS();

	SetValue(NewMode, EGameSettingChangeReason::Change);
}

int32 UOtterSettingValueDiscrete_MobileFPSType::GetDiscreteOptionIndex() const
{
	TArray<int32> FPSOptionsModes;
	FPSOptions.GenerateKeyArray(FPSOptionsModes);
	return FPSOptionsModes.IndexOfByKey(GetValue());
}

TArray<FText> UOtterSettingValueDiscrete_MobileFPSType::GetDiscreteOptions() const
{
	TArray<FText> Options;
	FPSOptions.GenerateValueArray(Options);

	return Options;
}

int32 UOtterSettingValueDiscrete_MobileFPSType::GetValue() const
{
	return UOtterSettingsLocal::Get()->GetDesiredMobileFrameRateLimit();
}

void UOtterSettingValueDiscrete_MobileFPSType::SetValue(int32 NewLimitFPS, EGameSettingChangeReason InReason)
{
	UOtterSettingsLocal::Get()->SetDesiredMobileFrameRateLimit(NewLimitFPS);

	NotifySettingChanged(InReason);
}

#undef LOCTEXT_NAMESPACE

