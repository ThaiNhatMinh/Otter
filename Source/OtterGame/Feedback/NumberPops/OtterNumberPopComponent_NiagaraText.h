// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OtterNumberPopComponent.h"

#include "OtterNumberPopComponent_NiagaraText.generated.h"

class UOtterDamagePopStyleNiagara;
class UNiagaraComponent;
class UObject;

UCLASS(Blueprintable)
class UOtterNumberPopComponent_NiagaraText : public UOtterNumberPopComponent
{
	GENERATED_BODY()

public:

	UOtterNumberPopComponent_NiagaraText(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UOtterNumberPopComponent interface
	virtual void AddNumberPop(const FOtterNumberPopRequest& NewRequest) override;
	//~End of UOtterNumberPopComponent interface

protected:
	
	TArray<int32> DamageNumberArray;

	/** Style patterns to attempt to apply to the incoming number pops */
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UOtterDamagePopStyleNiagara> Style;

	//Niagara Component used to display the damage
	UPROPERTY(EditDefaultsOnly, Category = "Number Pop|Style")
	TObjectPtr<UNiagaraComponent> NiagaraComp;
};
