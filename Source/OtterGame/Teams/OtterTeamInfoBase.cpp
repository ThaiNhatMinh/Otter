// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterTeamInfoBase.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Teams/OtterTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterTeamInfoBase)

class FLifetimeProperty;

AOtterTeamInfoBase::AOtterTeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(INDEX_NONE)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void AOtterTeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}

void AOtterTeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamSubsystem();
}

void AOtterTeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		UOtterTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UOtterTeamSubsystem>();
		if (TeamSubsystem)
		{
			// EndPlay can happen at weird times where the subsystem has already been destroyed
			TeamSubsystem->UnregisterTeamInfo(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AOtterTeamInfoBase::RegisterWithTeamSubsystem(UOtterTeamSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}

void AOtterTeamInfoBase::TryRegisterWithTeamSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		UOtterTeamSubsystem* TeamSubsystem = GetWorld()->GetSubsystem<UOtterTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			RegisterWithTeamSubsystem(TeamSubsystem);
		}
	}
}

void AOtterTeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamSubsystem();
}

void AOtterTeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamSubsystem();
}

