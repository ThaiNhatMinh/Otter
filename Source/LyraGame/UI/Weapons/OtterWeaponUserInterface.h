// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "OtterWeaponUserInterface.generated.h"

class UOtterWeaponInstance;
class UObject;
struct FGeometry;

UCLASS()
class UOtterWeaponUserInterface : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UOtterWeaponUserInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnWeaponChanged(UOtterWeaponInstance* OldWeapon, UOtterWeaponInstance* NewWeapon);

private:
	void RebuildWidgetFromWeapon();

private:
	UPROPERTY(Transient)
	TObjectPtr<UOtterWeaponInstance> CurrentInstance;
};
