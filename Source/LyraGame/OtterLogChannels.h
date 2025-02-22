// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtter, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterExperience, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterAbilitySystem, Log, All);
LYRAGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogOtterTeams, Log, All);

LYRAGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
