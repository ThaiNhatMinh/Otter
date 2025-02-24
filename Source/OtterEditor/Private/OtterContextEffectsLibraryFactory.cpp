// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterContextEffectsLibraryFactory.h"

#include "Feedback/ContextEffects/OtterContextEffectsLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterContextEffectsLibraryFactory)

class FFeedbackContext;
class UClass;
class UObject;

UOtterContextEffectsLibraryFactory::UOtterContextEffectsLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UOtterContextEffectsLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UOtterContextEffectsLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UOtterContextEffectsLibrary* OtterContextEffectsLibrary = NewObject<UOtterContextEffectsLibrary>(InParent, Name, Flags);

	return OtterContextEffectsLibrary;
}
