// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameSession)


AOtterGameSession::AOtterGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool AOtterGameSession::ProcessAutoLogin()
{
	// This is actually handled in OtterGameMode::TryDedicatedServerLogin
	return true;
}

void AOtterGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AOtterGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

