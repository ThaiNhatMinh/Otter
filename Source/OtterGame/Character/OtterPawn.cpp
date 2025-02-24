// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPawn.h"

#include "GameFramework/Controller.h"
#include "OtterLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPawn)

class FLifetimeProperty;
class UObject;

AOtterPawn::AOtterPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AOtterPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);
}

void AOtterPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOtterPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOtterPawn::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	// Grab the current team ID and listen for future changes
	if (IOtterTeamAgentInterface* ControllerAsTeamProvider = Cast<IOtterTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetOnTeamIndexChangedDelegate().AddUObject(this, &ThisClass::OnControllerChangedTeam);
	}
}

void AOtterPawn::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IOtterTeamAgentInterface* ControllerAsTeamProvider = Cast<IOtterTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetOnTeamIndexChangedDelegate().RemoveAll(this);
	}

	Super::UnPossessed();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
}

void AOtterPawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;
		MyTeamID = NewTeamID;
	}
	else
	{
		UE_LOG(LogOtterTeams, Error, TEXT("You can't set the team ID on a pawn (%s) except on the authority"), *GetPathNameSafe(this));
	}
}

FGenericTeamId AOtterPawn::GetGenericTeamId() const
{
	return MyTeamID;
}

void AOtterPawn::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
}

void AOtterPawn::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
}

