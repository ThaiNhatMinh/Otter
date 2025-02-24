// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"

#include "OtterGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct FOtterGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FOtterGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FOtterGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FOtterGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static OTTERGAME_API FOtterGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FOtterGameplayEffectContext* NewContext = new FOtterGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FOtterGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const;

};

template<>
struct TStructOpsTypeTraits<FOtterGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FOtterGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

