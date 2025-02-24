// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

OTTERGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtter, Log, All);
OTTERGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterExperience, Log, All);
OTTERGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterAbilitySystem, Log, All);
OTTERGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterTeams, Log, All);

OTTERGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
