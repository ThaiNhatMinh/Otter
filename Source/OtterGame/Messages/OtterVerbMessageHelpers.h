// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "OtterVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FOtterVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class OTTERGAME_API UOtterVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Otter")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Otter")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Otter")
	static FGameplayCueParameters VerbMessageToCueParameters(const FOtterVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Otter")
	static FOtterVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
