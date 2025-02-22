// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "OtterPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UOtterUICameraManagerComponent* UOtterUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (AOtterPlayerCameraManager* PCCamera = Cast<AOtterPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UOtterUICameraManagerComponent::UOtterUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UOtterUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOtterUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<AOtterPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UOtterUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UOtterUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UOtterUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
