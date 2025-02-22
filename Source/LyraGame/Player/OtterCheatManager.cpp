// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterCheatManager.h"
#include "GameFramework/Pawn.h"
#include "OtterPlayerController.h"
#include "OtterDebugCameraController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/Console.h"
#include "GameFramework/HUD.h"
#include "System/OtterAssetManager.h"
#include "System/OtterGameData.h"
#include "OtterGameplayTags.h"
#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/OtterHealthComponent.h"
#include "Character/OtterPawnExtensionComponent.h"
#include "System/OtterSystemStatics.h"
#include "Development/OtterDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCheatManager)

DEFINE_LOG_CATEGORY(LogOtterCheat);

namespace OtterCheat
{
	static const FName NAME_Fixed = FName(TEXT("Fixed"));
	
	static bool bEnableDebugCameraCycling = false;
	static FAutoConsoleVariableRef CVarEnableDebugCameraCycling(
		TEXT("OtterCheat.EnableDebugCameraCycling"),
		bEnableDebugCameraCycling,
		TEXT("If true then you can cycle the debug camera while running the game."),
		ECVF_Cheat);

	static bool bStartInGodMode = false;
	static FAutoConsoleVariableRef CVarStartInGodMode(
		TEXT("OtterCheat.StartInGodMode"),
		bStartInGodMode,
		TEXT("If true then the God cheat will be applied on begin play"),
		ECVF_Cheat);
};


UOtterCheatManager::UOtterCheatManager()
{
	DebugCameraControllerClass = AOtterDebugCameraController::StaticClass();
}

void UOtterCheatManager::InitCheatManager()
{
	Super::InitCheatManager();

#if WITH_EDITOR
	if (GIsEditor)
	{
		APlayerController* PC = GetOuterAPlayerController();
		for (const FOtterCheatToRun& CheatRow : GetDefault<UOtterDeveloperSettings>()->CheatsToRun)
		{
			if (CheatRow.Phase == ECheatExecutionTime::OnCheatManagerCreated)
			{
				PC->ConsoleCommand(CheatRow.Cheat, /*bWriteToLog=*/ true);
			}
		}
	}
#endif

	if (OtterCheat::bStartInGodMode)
	{
		God();	
	}
}

void UOtterCheatManager::CheatOutputText(const FString& TextToOutput)
{
#if USING_CHEAT_MANAGER
	// Output to the console.
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->ViewportConsole)
	{
		GEngine->GameViewport->ViewportConsole->OutputText(TextToOutput);
	}

	// Output to log.
	UE_LOG(LogOtterCheat, Display, TEXT("%s"), *TextToOutput);
#endif // USING_CHEAT_MANAGER
}

void UOtterCheatManager::Cheat(const FString& Msg)
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		OtterPC->ServerCheat(Msg.Left(128));
	}
}

void UOtterCheatManager::CheatAll(const FString& Msg)
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		OtterPC->ServerCheatAll(Msg.Left(128));
	}
}

void UOtterCheatManager::PlayNextGame()
{
	UOtterSystemStatics::PlayNextGame(this);
}

void UOtterCheatManager::EnableDebugCamera()
{
	Super::EnableDebugCamera();
}

void UOtterCheatManager::DisableDebugCamera()
{
	FVector DebugCameraLocation;
	FRotator DebugCameraRotation;

	ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* OriginalPC = nullptr;

	if (DebugCC)
	{
		OriginalPC = DebugCC->OriginalControllerRef;
		DebugCC->GetPlayerViewPoint(DebugCameraLocation, DebugCameraRotation);
	}

	Super::DisableDebugCamera();

	if (OriginalPC && OriginalPC->PlayerCameraManager && (OriginalPC->PlayerCameraManager->CameraStyle == OtterCheat::NAME_Fixed))
	{
		OriginalPC->SetInitialLocationAndRotation(DebugCameraLocation, DebugCameraRotation);

		OriginalPC->PlayerCameraManager->ViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->ViewTarget.POV.Rotation = DebugCameraRotation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Location = DebugCameraLocation;
		OriginalPC->PlayerCameraManager->PendingViewTarget.POV.Rotation = DebugCameraRotation;
	}
}

bool UOtterCheatManager::InDebugCamera() const
{
	return (Cast<ADebugCameraController>(GetOuter()) ? true : false);
}

void UOtterCheatManager::EnableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(OtterCheat::NAME_Fixed);
	}
}

void UOtterCheatManager::DisableFixedCamera()
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		PC->SetCameraMode(NAME_Default);
	}
}

bool UOtterCheatManager::InFixedCamera() const
{
	const ADebugCameraController* DebugCC = Cast<ADebugCameraController>(GetOuter());
	const APlayerController* PC = (DebugCC ? ToRawPtr(DebugCC->OriginalControllerRef) : GetOuterAPlayerController());

	if (PC && PC->PlayerCameraManager)
	{
		return (PC->PlayerCameraManager->CameraStyle == OtterCheat::NAME_Fixed);
	}

	return false;
}

void UOtterCheatManager::ToggleFixedCamera()
{
	if (InFixedCamera())
	{
		DisableFixedCamera();
	}
	else
	{
		EnableFixedCamera();
	}
}

void UOtterCheatManager::CycleDebugCameras()
{
	if (!OtterCheat::bEnableDebugCameraCycling)
	{
		return;
	}
	
	if (InDebugCamera())
	{
		EnableFixedCamera();
		DisableDebugCamera();
	}
	else if (InFixedCamera())
	{
		DisableFixedCamera();
		DisableDebugCamera();
	}
	else
	{
		EnableDebugCamera();
		DisableFixedCamera();
	}
}

void UOtterCheatManager::CycleAbilitySystemDebug()
{
	APlayerController* PC = Cast<APlayerController>(GetOuterAPlayerController());

	if (PC && PC->MyHUD)
	{
		if (!PC->MyHUD->bShowDebugInfo || !PC->MyHUD->DebugDisplay.Contains(TEXT("AbilitySystem")))
		{
			PC->MyHUD->ShowDebug(TEXT("AbilitySystem"));
		}

		PC->ConsoleCommand(TEXT("AbilitySystem.Debug.NextCategory"));
	}
}

void UOtterCheatManager::CancelActivatedAbilities()
{
	if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
	{
		const bool bReplicateCancelAbility = true;
		OtterASC->CancelInputActivatedAbilities(bReplicateCancelAbility);
	}
}

void UOtterCheatManager::AddTagToSelf(FString TagName)
{
	FGameplayTag Tag = OtterGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
		{
			OtterASC->AddDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogOtterCheat, Display, TEXT("AddTagToSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UOtterCheatManager::RemoveTagFromSelf(FString TagName)
{
	FGameplayTag Tag = OtterGameplayTags::FindTagByString(TagName, true);
	if (Tag.IsValid())
	{
		if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
		{
			OtterASC->RemoveDynamicTagGameplayEffect(Tag);
		}
	}
	else
	{
		UE_LOG(LogOtterCheat, Display, TEXT("RemoveTagFromSelf: Could not find any tag matching [%s]."), *TagName);
	}
}

void UOtterCheatManager::DamageSelf(float DamageAmount)
{
	if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerDamage(OtterASC, DamageAmount);
	}
}

void UOtterCheatManager::DamageTarget(float DamageAmount)
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		if (OtterPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			OtterPC->ServerCheat(FString::Printf(TEXT("DamageTarget %.2f"), DamageAmount));
			return;
		}

		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(OtterPC, TargetHitResult);

		if (UOtterAbilitySystemComponent* OtterTargetASC = Cast<UOtterAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerDamage(OtterTargetASC, DamageAmount);
		}
	}
}

void UOtterCheatManager::ApplySetByCallerDamage(UOtterAbilitySystemComponent* OtterASC, float DamageAmount)
{
	check(OtterASC);

	TSubclassOf<UGameplayEffect> DamageGE = UOtterAssetManager::GetSubclass(UOtterGameData::Get().DamageGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = OtterASC->MakeOutgoingSpec(DamageGE, 1.0f, OtterASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(OtterGameplayTags::SetByCaller_Damage, DamageAmount);
		OtterASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UOtterCheatManager::HealSelf(float HealAmount)
{
	if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
	{
		ApplySetByCallerHeal(OtterASC, HealAmount);
	}
}

void UOtterCheatManager::HealTarget(float HealAmount)
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		FHitResult TargetHitResult;
		AActor* TargetActor = GetTarget(OtterPC, TargetHitResult);

		if (UOtterAbilitySystemComponent* OtterTargetASC = Cast<UOtterAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor)))
		{
			ApplySetByCallerHeal(OtterTargetASC, HealAmount);
		}
	}
}

void UOtterCheatManager::ApplySetByCallerHeal(UOtterAbilitySystemComponent* OtterASC, float HealAmount)
{
	check(OtterASC);

	TSubclassOf<UGameplayEffect> HealGE = UOtterAssetManager::GetSubclass(UOtterGameData::Get().HealGameplayEffect_SetByCaller);
	FGameplayEffectSpecHandle SpecHandle = OtterASC->MakeOutgoingSpec(HealGE, 1.0f, OtterASC->MakeEffectContext());

	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(OtterGameplayTags::SetByCaller_Heal, HealAmount);
		OtterASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

UOtterAbilitySystemComponent* UOtterCheatManager::GetPlayerAbilitySystemComponent() const
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		return OtterPC->GetOtterAbilitySystemComponent();
	}
	return nullptr;
}

void UOtterCheatManager::DamageSelfDestruct()
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
 		if (const UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(OtterPC->GetPawn()))
		{
			if (PawnExtComp->HasReachedInitState(OtterGameplayTags::InitState_GameplayReady))
			{
				if (UOtterHealthComponent* HealthComponent = UOtterHealthComponent::FindHealthComponent(OtterPC->GetPawn()))
				{
					HealthComponent->DamageSelfDestruct();
				}
			}
		}
	}
}

void UOtterCheatManager::God()
{
	if (AOtterPlayerController* OtterPC = Cast<AOtterPlayerController>(GetOuterAPlayerController()))
	{
		if (OtterPC->GetNetMode() == NM_Client)
		{
			// Automatically send cheat to server for convenience.
			OtterPC->ServerCheat(FString::Printf(TEXT("God")));
			return;
		}

		if (UOtterAbilitySystemComponent* OtterASC = OtterPC->GetOtterAbilitySystemComponent())
		{
			const FGameplayTag Tag = OtterGameplayTags::Cheat_GodMode;
			const bool bHasTag = OtterASC->HasMatchingGameplayTag(Tag);

			if (bHasTag)
			{
				OtterASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				OtterASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

void UOtterCheatManager::UnlimitedHealth(int32 Enabled)
{
	if (UOtterAbilitySystemComponent* OtterASC = GetPlayerAbilitySystemComponent())
	{
		const FGameplayTag Tag = OtterGameplayTags::Cheat_UnlimitedHealth;
		const bool bHasTag = OtterASC->HasMatchingGameplayTag(Tag);

		if ((Enabled == -1) || ((Enabled > 0) && !bHasTag) || ((Enabled == 0) && bHasTag))
		{
			if (bHasTag)
			{
				OtterASC->RemoveDynamicTagGameplayEffect(Tag);
			}
			else
			{
				OtterASC->AddDynamicTagGameplayEffect(Tag);
			}
		}
	}
}

