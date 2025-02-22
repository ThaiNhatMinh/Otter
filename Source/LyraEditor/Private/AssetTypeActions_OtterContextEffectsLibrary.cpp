// Copyright Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_OtterContextEffectsLibrary.h"

#include "Feedback/ContextEffects/OtterContextEffectsLibrary.h"

class UClass;

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UClass* FAssetTypeActions_OtterContextEffectsLibrary::GetSupportedClass() const
{
	return UOtterContextEffectsLibrary::StaticClass();
}

#undef LOCTEXT_NAMESPACE
