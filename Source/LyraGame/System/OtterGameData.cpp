// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameData.h"
#include "OtterAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameData)

UOtterGameData::UOtterGameData()
{
}

const UOtterGameData& UOtterGameData::UOtterGameData::Get()
{
	return UOtterAssetManager::Get().GetGameData();
}
