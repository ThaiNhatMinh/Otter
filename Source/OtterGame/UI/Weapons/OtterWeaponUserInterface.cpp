// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterWeaponUserInterface.h"

#include "Equipment/OtterEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Weapons/OtterWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterWeaponUserInterface)

struct FGeometry;

UOtterWeaponUserInterface::UOtterWeaponUserInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOtterWeaponUserInterface::NativeConstruct()
{
	Super::NativeConstruct();
}

void UOtterWeaponUserInterface::NativeDestruct()
{
	Super::NativeDestruct();
}

void UOtterWeaponUserInterface::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (UOtterEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UOtterEquipmentManagerComponent>())
		{
			if (UOtterWeaponInstance* NewInstance = EquipmentManager->GetFirstInstanceOfType<UOtterWeaponInstance>())
			{
				if (NewInstance != CurrentInstance && NewInstance->GetInstigator() != nullptr)
				{
					UOtterWeaponInstance* OldWeapon = CurrentInstance;
					CurrentInstance = NewInstance;
					RebuildWidgetFromWeapon();
					OnWeaponChanged(OldWeapon, CurrentInstance);
				}
			}
		}
	}
}

void UOtterWeaponUserInterface::RebuildWidgetFromWeapon()
{
	
}

