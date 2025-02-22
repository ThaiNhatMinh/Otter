// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameplayAbility_FromEquipment.h"
#include "OtterEquipmentInstance.h"
#include "Inventory/OtterInventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameplayAbility_FromEquipment)

UOtterGameplayAbility_FromEquipment::UOtterGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UOtterEquipmentInstance* UOtterGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UOtterEquipmentInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UOtterInventoryItemInstance* UOtterGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (UOtterEquipmentInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UOtterInventoryItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UOtterGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

PRAGMA_DISABLE_DEPRECATION_WARNINGS
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	{
		Context.AddError(NSLOCTEXT("Otter", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
