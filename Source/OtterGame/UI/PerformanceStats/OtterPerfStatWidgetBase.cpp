// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPerfStatWidgetBase.h"

#include "Engine/GameInstance.h"
#include "Performance/OtterPerformanceStatSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPerfStatWidgetBase)

//////////////////////////////////////////////////////////////////////
// UOtterPerfStatWidgetBase

UOtterPerfStatWidgetBase::UOtterPerfStatWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

double UOtterPerfStatWidgetBase::FetchStatValue()
{
	if (CachedStatSubsystem == nullptr)
	{
		if (UWorld* World = GetWorld())
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				CachedStatSubsystem = GameInstance->GetSubsystem<UOtterPerformanceStatSubsystem>();
			}
		}
	}

	if (CachedStatSubsystem)
	{
		return CachedStatSubsystem->GetCachedStat(StatToDisplay);
	}
	else
	{
		return 0.0;
	}
}

