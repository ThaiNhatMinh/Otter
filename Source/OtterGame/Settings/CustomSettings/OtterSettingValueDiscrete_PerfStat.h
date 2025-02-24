// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameSettingValueDiscrete.h"

#include "OtterSettingValueDiscrete_PerfStat.generated.h"

enum class EOtterDisplayablePerformanceStat : uint8;
enum class EOtterStatDisplayMode : uint8;

class UObject;

UCLASS()
class UOtterSettingValueDiscrete_PerfStat : public UGameSettingValueDiscrete
{
	GENERATED_BODY()
	
public:

	UOtterSettingValueDiscrete_PerfStat();

	void SetStat(EOtterDisplayablePerformanceStat InStat);

	/** UGameSettingValue */
	virtual void StoreInitial() override;
	virtual void ResetToDefault() override;
	virtual void RestoreToInitial() override;

	/** UGameSettingValueDiscrete */
	virtual void SetDiscreteOptionByIndex(int32 Index) override;
	virtual int32 GetDiscreteOptionIndex() const override;
	virtual TArray<FText> GetDiscreteOptions() const override;

protected:
	/** UGameSettingValue */
	virtual void OnInitialized() override;
	
	void AddMode(FText&& Label, EOtterStatDisplayMode Mode);
protected:
	TArray<FText> Options;
	TArray<EOtterStatDisplayMode> DisplayModes;

	EOtterDisplayablePerformanceStat StatToDisplay;
	EOtterStatDisplayMode InitialMode;
};
