// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DebugCameraController.h"

#include "OtterDebugCameraController.generated.h"

class UObject;


/**
 * AOtterDebugCameraController
 *
 *	Used for controlling the debug camera when it is enabled via the cheat manager.
 */
UCLASS()
class AOtterDebugCameraController : public ADebugCameraController
{
	GENERATED_BODY()

public:

	AOtterDebugCameraController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void AddCheats(bool bForce) override;
};
