// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonGameViewportClient.h"

#include "OtterGameViewportClient.generated.h"

class UGameInstance;
class UObject;

UCLASS(BlueprintType)
class UOtterGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UOtterGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
};
