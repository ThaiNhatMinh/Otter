// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterEquipmentDefinition.h"
#include "OtterEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterEquipmentDefinition)

UOtterEquipmentDefinition::UOtterEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UOtterEquipmentInstance::StaticClass();
}

