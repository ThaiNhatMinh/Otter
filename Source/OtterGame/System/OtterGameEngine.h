// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/GameEngine.h"

#include "OtterGameEngine.generated.h"

class IEngineLoop;
class UObject;


UCLASS()
class UOtterGameEngine : public UGameEngine
{
	GENERATED_BODY()

public:

	UOtterGameEngine(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void Init(IEngineLoop* InEngineLoop) override;
};
