// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/OtterLocalPlayer.h"
#include "Settings/OtterSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterInputComponent)

class UOtterInputConfig;

UOtterInputComponent::UOtterInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UOtterInputComponent::AddInputMappings(const UOtterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UOtterInputComponent::RemoveInputMappings(const UOtterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UOtterInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
