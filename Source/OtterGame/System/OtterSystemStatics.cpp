// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterSystemStatics.h"
#include "Engine/AssetManagerTypes.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
#include "OtterLogChannels.h"
#include "Components/MeshComponent.h"
#include "GameModes/OtterUserFacingExperienceDefinition.h"
#include "CommonInputModeTypes.h"
#include "Input/CommonUIActionRouterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterSystemStatics)

TSoftObjectPtr<UObject> UOtterSystemStatics::GetTypedSoftObjectReferenceFromPrimaryAssetId(FPrimaryAssetId PrimaryAssetId, TSubclassOf<UObject> ExpectedAssetType)
{
	if (UAssetManager* Manager = UAssetManager::GetIfInitialized())
	{
		FPrimaryAssetTypeInfo Info;
		if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info) && !Info.bHasBlueprintClasses)
		{
			if (UClass* AssetClass = Info.AssetBaseClassLoaded)
			{
				if ((ExpectedAssetType == nullptr) || !AssetClass->IsChildOf(ExpectedAssetType))
				{
					return nullptr;
				}
			}
			else
			{
				UE_LOG(LogOtter, Warning, TEXT("GetTypedSoftObjectReferenceFromPrimaryAssetId(%s, %s) - AssetBaseClassLoaded was unset so we couldn't validate it, returning null"),
					*PrimaryAssetId.ToString(),
					*GetPathNameSafe(*ExpectedAssetType));
			}

			return TSoftObjectPtr<UObject>(Manager->GetPrimaryAssetPath(PrimaryAssetId));
		}
	}
	return nullptr;
}

FPrimaryAssetId UOtterSystemStatics::GetPrimaryAssetIdFromUserFacingExperienceName(const FString& AdvertisedExperienceID)
{
	const FPrimaryAssetType Type(UOtterUserFacingExperienceDefinition::StaticClass()->GetFName());
	return FPrimaryAssetId(Type, FName(*AdvertisedExperienceID));
}

void UOtterSystemStatics::PlayNextGame(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return;
	}

	const FWorldContext& WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
	FURL LastURL = WorldContext.LastURL;

#if WITH_EDITOR
	// To transition during PIE we need to strip the PIE prefix from maps.
	LastURL.Map = UWorld::StripPIEPrefixFromPackageName(LastURL.Map, WorldContext.World()->StreamingLevelsPrefix);
#endif

	// Add seamless travel option as we want to keep clients connected. This will fall back to hard travel if seamless is disabled
	LastURL.AddOption(TEXT("SeamlessTravel"));

	FString URL = LastURL.ToString();
	// If we don't remove the host/port info the server travel will fail.
	URL.RemoveFromStart(LastURL.GetHostPortString());
	
	const bool bAbsolute = false; // we want to use TRAVEL_Relative
	const bool bShouldSkipGameNotify = false;
	World->ServerTravel(URL, bAbsolute, bShouldSkipGameNotify);
}

void UOtterSystemStatics::SetScalarParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName, const float ParameterValue, bool bIncludeChildActors)
{
	if (TargetActor != nullptr)
	{
		TargetActor->ForEachComponent<UMeshComponent>(bIncludeChildActors, [=](UMeshComponent* InComponent)
		{
			InComponent->SetScalarParameterValueOnMaterials(ParameterName, ParameterValue);
		});
	}
}

void UOtterSystemStatics::SetVectorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName, const FVector ParameterValue, bool bIncludeChildActors)
{
	if (TargetActor != nullptr)
	{
		TargetActor->ForEachComponent<UMeshComponent>(bIncludeChildActors, [=](UMeshComponent* InComponent)
		{
			InComponent->SetVectorParameterValueOnMaterials(ParameterName, ParameterValue);
		});
	}
}

void UOtterSystemStatics::SetColorParameterValueOnAllMeshComponents(AActor* TargetActor, const FName ParameterName, const FLinearColor ParameterValue, bool bIncludeChildActors)
{
	SetVectorParameterValueOnAllMeshComponents(TargetActor, ParameterName, FVector(ParameterValue), bIncludeChildActors);
}

TArray<UActorComponent*> UOtterSystemStatics::FindComponentsByClass(AActor* TargetActor, TSubclassOf<UActorComponent> ComponentClass, bool bIncludeChildActors)
{
	TArray<UActorComponent*> Components;
	if (TargetActor != nullptr)
	{
		TargetActor->GetComponents(ComponentClass, /*out*/ Components, bIncludeChildActors);

	}
	return MoveTemp(Components);
}

void UOtterSystemStatics::SetInputMode(const APlayerController *PlayerController, ECommonInputMode CommonInputMode)
{
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogOtter, Error, TEXT("Cannot SetInputMode for non-local player [%s]"), *PlayerController->GetName());
		return;
	}

	UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>();
	if (!ActionRouter)
	{
		UE_LOG(LogOtter, Error, TEXT("CommonUIActionRouter is not available, cannot SetInputMode"));
		return;
	}

	FUIInputConfig InputConfig;
	if (CommonInputMode == ECommonInputMode::Game)
	{
		// Game mode means invisible mouse, permanently captured
		constexpr bool bHideCursorDuringViewportCapture = true;
		InputConfig = FUIInputConfig(CommonInputMode, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, bHideCursorDuringViewportCapture);
	}
	else
	{
		// Menu or All modes mean visible mouse, not permanently captured
		constexpr bool bHideCursorDuringViewportCapture = false;
		InputConfig = FUIInputConfig(CommonInputMode, EMouseCaptureMode::CaptureDuringMouseDown, bHideCursorDuringViewportCapture);
	}

	ActionRouter->SetActiveUIInputConfig(InputConfig);
}
