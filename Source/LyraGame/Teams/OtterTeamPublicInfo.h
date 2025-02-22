// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OtterTeamInfoBase.h"

#include "OtterTeamPublicInfo.generated.h"

class UOtterTeamCreationComponent;
class UOtterTeamDisplayAsset;
class UObject;
struct FFrame;

UCLASS()
class AOtterTeamPublicInfo : public AOtterTeamInfoBase
{
	GENERATED_BODY()

	friend UOtterTeamCreationComponent;

public:
	AOtterTeamPublicInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UOtterTeamDisplayAsset* GetTeamDisplayAsset() const { return TeamDisplayAsset; }

private:
	UFUNCTION()
	void OnRep_TeamDisplayAsset();

	void SetTeamDisplayAsset(TObjectPtr<UOtterTeamDisplayAsset> NewDisplayAsset);

private:
	UPROPERTY(ReplicatedUsing=OnRep_TeamDisplayAsset)
	TObjectPtr<UOtterTeamDisplayAsset> TeamDisplayAsset;
};
