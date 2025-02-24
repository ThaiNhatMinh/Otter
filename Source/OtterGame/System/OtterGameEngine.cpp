// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameEngine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameEngine)

class IEngineLoop;


UOtterGameEngine::UOtterGameEngine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOtterGameEngine::Init(IEngineLoop* InEngineLoop)
{
	Super::Init(InEngineLoop);
}

