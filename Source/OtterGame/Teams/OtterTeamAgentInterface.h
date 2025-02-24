// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"
#include "UObject/Object.h"

#include "UObject/WeakObjectPtr.h"
#include "OtterTeamAgentInterface.generated.h"

template <typename InterfaceType> class TScriptInterface;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnOtterTeamIndexChangedDelegate, UObject* ObjectChangingTeam, int32 OldTeamID, int32 NewTeamID);

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)ID;
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

/** Interface for actors which can be associated with teams */
UINTERFACE(meta=(CannotImplementInterfaceInBlueprint))
class UOtterTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_UINTERFACE_BODY()
};

class OTTERGAME_API IOtterTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_IINTERFACE_BODY()

	FOnOtterTeamIndexChangedDelegate& GetOnTeamIndexChangedDelegate() { return OnOtterTeamIndexChangedDelegate; }

private:
	FOnOtterTeamIndexChangedDelegate OnOtterTeamIndexChangedDelegate;
};
