// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterSettingScreen.h"

#include "Input/CommonUIInputTypes.h"
#include "Player/OtterLocalPlayer.h"
#include "Settings/OtterGameSettingRegistry.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterSettingScreen)

class UGameSettingRegistry;

void UOtterSettingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackHandle = RegisterUIActionBinding(FBindUIActionArgs(BackInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction)));
	ApplyHandle = RegisterUIActionBinding(FBindUIActionArgs(ApplyInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleApplyAction)));
	CancelChangesHandle = RegisterUIActionBinding(FBindUIActionArgs(CancelChangesInputActionData, true, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleCancelChangesAction)));
}

UGameSettingRegistry* UOtterSettingScreen::CreateRegistry()
{
	UOtterGameSettingRegistry* NewRegistry = NewObject<UOtterGameSettingRegistry>();

	if (UOtterLocalPlayer* LocalPlayer = CastChecked<UOtterLocalPlayer>(GetOwningLocalPlayer()))
	{
		NewRegistry->Initialize(LocalPlayer);
	}

	return NewRegistry;
}

void UOtterSettingScreen::HandleBackAction()
{
	if (AttemptToPopNavigation())
	{
		return;
	}

	ApplyChanges();

	DeactivateWidget();
}

void UOtterSettingScreen::HandleApplyAction()
{
	ApplyChanges();
}

void UOtterSettingScreen::HandleCancelChangesAction()
{
	CancelChanges();
}

void UOtterSettingScreen::OnSettingsDirtyStateChanged_Implementation(bool bSettingsDirty)
{
	if (bSettingsDirty)
	{
		if (!GetActionBindings().Contains(ApplyHandle))
		{
			AddActionBinding(ApplyHandle);
		}
		if (!GetActionBindings().Contains(CancelChangesHandle))
		{
			AddActionBinding(CancelChangesHandle);
		}
	}
	else
	{
		RemoveActionBinding(ApplyHandle);
		RemoveActionBinding(CancelChangesHandle);
	}
}
