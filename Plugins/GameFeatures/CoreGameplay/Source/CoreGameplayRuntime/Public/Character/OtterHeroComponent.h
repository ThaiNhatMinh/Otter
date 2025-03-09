// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OtterHeroComponentBase.h"
#include "OtterHeroComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class COREGAMEPLAYRUNTIME_API UOtterHeroComponent : public UOtterHeroComponentBase
{
	GENERATED_BODY()
public:
	UOtterHeroComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializePlayerInput(const class UOtterInputConfig*, class UOtterInputComponent* InputComponent);
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
};
