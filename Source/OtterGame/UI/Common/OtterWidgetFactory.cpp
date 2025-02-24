// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterWidgetFactory.h"
#include "Templates/SubclassOf.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterWidgetFactory)

class UUserWidget;

TSubclassOf<UUserWidget> UOtterWidgetFactory::FindWidgetClassForData_Implementation(const UObject* Data) const
{
	return TSubclassOf<UUserWidget>();
}
