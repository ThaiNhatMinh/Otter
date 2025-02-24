// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "OtterDamagePopStyle.generated.h"

class UObject;
class UStaticMesh;

UCLASS()
class UOtterDamagePopStyle : public UDataAsset
{
	GENERATED_BODY()

public:

	UOtterDamagePopStyle();

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FString DisplayText;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop")
	FGameplayTagQuery MatchPattern;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideColor))
	FLinearColor Color;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideColor))
	FLinearColor CriticalColor;

	UPROPERTY(EditDefaultsOnly, Category="DamagePop", meta=(EditCondition=bOverrideMesh))
	TObjectPtr<UStaticMesh> TextMesh;

	UPROPERTY()
	bool bOverrideColor = false;

	UPROPERTY()
	bool bOverrideMesh = false;
};
