// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterTeamStatics.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "OtterLogChannels.h"
#include "Teams/OtterTeamDisplayAsset.h"
#include "Teams/OtterTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterTeamStatics)

class UTexture;

//////////////////////////////////////////////////////////////////////

void UOtterTeamStatics::FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, UOtterTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet)
{
	bIsPartOfTeam = false;
	TeamId = INDEX_NONE;
	DisplayAsset = nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(Agent, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UOtterTeamSubsystem* TeamSubsystem = World->GetSubsystem<UOtterTeamSubsystem>())
		{
			TeamId = TeamSubsystem->FindTeamFromObject(Agent);
			if (TeamId != INDEX_NONE)
			{
				bIsPartOfTeam = true;

				DisplayAsset = TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);

				if ((DisplayAsset == nullptr) && bLogIfNotSet)
				{
					UE_LOG(LogOtterTeams, Log, TEXT("FindTeamFromObject(%s) called too early (found team %d but no display asset set yet"), *GetPathNameSafe(Agent), TeamId);
				}
			}
		}
		else
		{
			UE_LOG(LogOtterTeams, Error, TEXT("FindTeamFromObject(%s) failed: Team subsystem does not exist yet"), *GetPathNameSafe(Agent));
		}
	}
}

UOtterTeamDisplayAsset* UOtterTeamStatics::GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId)
{
	UOtterTeamDisplayAsset* Result = nullptr;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UOtterTeamSubsystem* TeamSubsystem = World->GetSubsystem<UOtterTeamSubsystem>())
		{
			return TeamSubsystem->GetTeamDisplayAsset(TeamId, INDEX_NONE);
		}
	}
	return Result;
}

float UOtterTeamStatics::GetTeamScalarWithFallback(UOtterTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue)
{
	if (DisplayAsset)
	{
		if (float* pValue = DisplayAsset->ScalarParameters.Find(ParameterName))
		{
			return *pValue;
		}
	}
	return DefaultValue;
}

FLinearColor UOtterTeamStatics::GetTeamColorWithFallback(UOtterTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue)
{
	if (DisplayAsset)
	{
		if (FLinearColor* pColor = DisplayAsset->ColorParameters.Find(ParameterName))
		{
			return *pColor;
		}
	}
	return DefaultValue;
}

UTexture* UOtterTeamStatics::GetTeamTextureWithFallback(UOtterTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue)
{
	if (DisplayAsset)
	{
		if (TObjectPtr<UTexture>* pTexture = DisplayAsset->TextureParameters.Find(ParameterName))
		{
			return *pTexture;
		}
	}
	return DefaultValue;
}

