// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OtterTeamInfoBase.h"

#include "OtterTeamPrivateInfo.generated.h"

class UObject;

UCLASS()
class AOtterTeamPrivateInfo : public AOtterTeamInfoBase
{
	GENERATED_BODY()

public:
	AOtterTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
