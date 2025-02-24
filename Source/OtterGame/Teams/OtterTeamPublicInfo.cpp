// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterTeamPublicInfo.h"

#include "Net/UnrealNetwork.h"
#include "Teams/OtterTeamInfoBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterTeamPublicInfo)

class FLifetimeProperty;

AOtterTeamPublicInfo::AOtterTeamPublicInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AOtterTeamPublicInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayAsset, COND_InitialOnly);
}

void AOtterTeamPublicInfo::SetTeamDisplayAsset(TObjectPtr<UOtterTeamDisplayAsset> NewDisplayAsset)
{
	check(HasAuthority());
	check(TeamDisplayAsset == nullptr);

	TeamDisplayAsset = NewDisplayAsset;

	TryRegisterWithTeamSubsystem();
}

void AOtterTeamPublicInfo::OnRep_TeamDisplayAsset()
{
	TryRegisterWithTeamSubsystem();
}

