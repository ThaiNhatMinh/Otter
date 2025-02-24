// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterCosmeticCheats.h"
#include "Cosmetics/OtterCharacterPartTypes.h"
#include "OtterControllerComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "System/OtterDevelopmentStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCosmeticCheats)

//////////////////////////////////////////////////////////////////////
// UOtterCosmeticCheats

UOtterCosmeticCheats::UOtterCosmeticCheats()
{
#if UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void UOtterCosmeticCheats::AddCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	if (UOtterControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		TSubclassOf<AActor> PartClass = UOtterDevelopmentStatics::FindClassByShortName<AActor>(AssetName);
		if (PartClass != nullptr)
		{
			FOtterCharacterPart Part;
			Part.PartClass = PartClass;

			CosmeticComponent->AddCheatPart(Part, bSuppressNaturalParts);
		}
	}
#endif	
}

void UOtterCosmeticCheats::ReplaceCharacterPart(const FString& AssetName, bool bSuppressNaturalParts)
{
	ClearCharacterPartOverrides();
	AddCharacterPart(AssetName, bSuppressNaturalParts);
}

void UOtterCosmeticCheats::ClearCharacterPartOverrides()
{
#if UE_WITH_CHEAT_MANAGER
	if (UOtterControllerComponent_CharacterParts* CosmeticComponent = GetCosmeticComponent())
	{
		CosmeticComponent->ClearCheatParts();
	}
#endif	
}

UOtterControllerComponent_CharacterParts* UOtterCosmeticCheats::GetCosmeticComponent() const
{
	if (APlayerController* PC = GetPlayerController())
	{
		return PC->FindComponentByClass<UOtterControllerComponent_CharacterParts>();
	}

	return nullptr;
}

