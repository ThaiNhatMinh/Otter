// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonUIActionRouterBase.h"
#include "OtterCommonUIActionRouterBase.generated.h"

/**
 * 
 */
UCLASS()
class OTTERGAME_API UOtterCommonUIActionRouterBase : public UCommonUIActionRouterBase
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/**
	 * Change the UI Input Config to match NewConfig
	 *
	 * This is the setter for the Input Mode for your game.
	 * This will be called any time any widget or other application code wants to
	 * change the input mode to some specific value.
	 *
	 * This should do nothing if the current config already equals NewConfig.
	 *
	 * If NewConfig is somehow different than the current config, apply the changes
	 * and broadcast input config change events.
	 *
	 * If you want to explicitly change the input mode to something new:
	 * @see UCommonUIActionRouterBase::SetActiveUIInputConfig(NewConfig)
	 *
	 * @param NewConfig The new Input Config to set (possibly the same as the current setting)
	 * @param bForceRefresh Whether to forcefully set the input mode even if we think it has not changed
	 */
	virtual void ApplyUIInputConfig(const FUIInputConfig& NewConfig, bool bForceRefresh) override;
};
