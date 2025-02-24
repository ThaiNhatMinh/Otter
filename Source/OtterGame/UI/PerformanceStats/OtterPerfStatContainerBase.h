// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "Performance/OtterPerformanceStatTypes.h"

#include "OtterPerfStatContainerBase.generated.h"

class UObject;
struct FFrame;

/**
 * UOtterPerfStatsContainerBase
 *
 * Panel that contains a set of UOtterPerfStatWidgetBase widgets and manages
 * their visibility based on user settings.
 */
 UCLASS(Abstract)
class UOtterPerfStatContainerBase : public UCommonUserWidget
{
public:
	UOtterPerfStatContainerBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	GENERATED_BODY()

	//~UUserWidget interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~End of UUserWidget interface

	UFUNCTION(BlueprintCallable)
	void UpdateVisibilityOfChildren();

protected:
	// Are we showing text or graph stats?
	UPROPERTY(EditAnywhere, Category=Display)
	EOtterStatDisplayMode StatDisplayModeFilter = EOtterStatDisplayMode::TextAndGraph;
};
