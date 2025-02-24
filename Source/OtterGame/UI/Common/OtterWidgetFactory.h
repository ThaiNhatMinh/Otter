// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/Object.h"

#include "OtterWidgetFactory.generated.h"

template <class TClass> class TSubclassOf;

class UUserWidget;
struct FFrame;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew)
class OTTERGAME_API UOtterWidgetFactory : public UObject
{
	GENERATED_BODY()

public:
	UOtterWidgetFactory() { }

	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UUserWidget> FindWidgetClassForData(const UObject* Data) const;
};
