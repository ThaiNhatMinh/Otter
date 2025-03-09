// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OtterPawnData.h"
#include "GameplayTagContainer.h"
#include "OtterCharacterData.generated.h"

struct FGameplayTag;
class UUserWidget;

/**
 * 
 */
UCLASS()
class COREGAMEPLAYRUNTIME_API UOtterCharacterData : public UOtterPawnData
{
	GENERATED_BODY()
public:
	// The slot ID where we should place this widget
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|UI")
	TMap<FGameplayTag, TSoftClassPtr<UUserWidget>> CharacterUI;
};
