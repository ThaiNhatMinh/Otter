// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/OtterCameraMode.h"
#include "Curves/CurveFloat.h"

#include "OtterCameraMode_TopDownArenaCamera.generated.h"

class UObject;


/**
 * UOtterCameraMode_TopDownArenaCamera
 *
 *	A basic third person camera mode that looks down at a fixed arena.
 */
UCLASS(Abstract, Blueprintable)
class UOtterCameraMode_TopDownArenaCamera : public UOtterCameraMode
{
	GENERATED_BODY()

public:

	UOtterCameraMode_TopDownArenaCamera();

protected:

	//~UOtterCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~End of UOtterCameraMode interface

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	float ArenaWidth;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	float ArenaHeight;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FRotator DefaultPivotRotation;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FRuntimeFloatCurve BoundsSizeToDistance;
};
