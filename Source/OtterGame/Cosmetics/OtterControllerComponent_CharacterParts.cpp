// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cosmetics/OtterControllerComponent_CharacterParts.h"
#include "Cosmetics/OtterCharacterPartTypes.h"
#include "Cosmetics/OtterPawnComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
#include "OtterCosmeticDeveloperSettings.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterControllerComponent_CharacterParts)

//////////////////////////////////////////////////////////////////////

UOtterControllerComponent_CharacterParts::UOtterControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOtterControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Listen for pawn possession changed events
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void UOtterControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UOtterPawnComponent_CharacterParts* UOtterControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UOtterPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UOtterControllerComponent_CharacterParts::AddCharacterPart(const FOtterCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, ECharacterPartSource::Natural);
}

void UOtterControllerComponent_CharacterParts::AddCharacterPartInternal(const FOtterCharacterPart& NewPart, ECharacterPartSource Source)
{
	FOtterControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void UOtterControllerComponent_CharacterParts::RemoveCharacterPart(const FOtterCharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (FOtterCharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void UOtterControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FOtterControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void UOtterControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Remove from the old pawn
	if (UOtterPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UOtterPawnComponent_CharacterParts>() : nullptr)
	{
		for (FOtterControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// Apply to the new pawn
	if (UOtterPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UOtterPawnComponent_CharacterParts>() : nullptr)
	{
		for (FOtterControllerCharacterPartEntry& Entry : CharacterParts)
		{
			// Don't readd if it's already there, this can get called with a null oldpawn
			if (!Entry.Handle.IsValid() && Entry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void UOtterControllerComponent_CharacterParts::ApplyDeveloperSettings()
{
#if UE_WITH_CHEAT_MANAGER
	const UOtterCosmeticDeveloperSettings* Settings = GetDefault<UOtterCosmeticDeveloperSettings>();

	// Suppress or unsuppress natural parts if needed
	const bool bSuppressNaturalParts = (Settings->CheatMode == ECosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);

	// Remove anything added by developer settings and re-add it
	UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaDeveloperSettingsCheat)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	// Add new parts
	for (const FOtterCharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
	{
		AddCharacterPartInternal(PartDesc, ECharacterPartSource::AppliedViaDeveloperSettingsCheat);
	}
#endif
}


void UOtterControllerComponent_CharacterParts::AddCheatPart(const FOtterCharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, ECharacterPartSource::AppliedViaCheatManager);
#endif
}

void UOtterControllerComponent_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// Remove anything added by cheat manager cheats
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void UOtterControllerComponent_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	UOtterPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (FOtterControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == ECharacterPartSource::Natural) && bSuppressed)
		{
			// Suppress
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = ECharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == ECharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// Unsuppress
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = ECharacterPartSource::Natural;
		}
	}
#endif
}

