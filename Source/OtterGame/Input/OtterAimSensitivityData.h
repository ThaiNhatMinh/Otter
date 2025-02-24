// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "OtterAimSensitivityData.generated.h"

enum class EOtterGamepadSensitivity : uint8;

class UObject;

/** Defines a set of gamepad sensitivity to a float value. */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Otter Aim Sensitivity Data", ShortTooltip = "Data asset used to define a map of Gamepad Sensitivty to a float value."))
class OTTERGAME_API UOtterAimSensitivityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOtterAimSensitivityData(const FObjectInitializer& ObjectInitializer);
	
	const float SensitivtyEnumToFloat(const EOtterGamepadSensitivity InSensitivity) const;
	
protected:
	/** Map of SensitivityMap settings to their corresponding float */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EOtterGamepadSensitivity, float> SensitivityMap;
};
