// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OtterWidgetFactory.h"
#include "Templates/SubclassOf.h"
#include "UObject/SoftObjectPtr.h"

#include "OtterWidgetFactory_Class.generated.h"

class UObject;
class UUserWidget;

UCLASS()
class OTTERGAME_API UOtterWidgetFactory_Class : public UOtterWidgetFactory
{
	GENERATED_BODY()

public:
	UOtterWidgetFactory_Class() { }

	virtual TSubclassOf<UUserWidget> FindWidgetClassForData_Implementation(const UObject* Data) const override;
	
protected:
	UPROPERTY(EditAnywhere, Category = ListEntries, meta = (AllowAbstract))
	TMap<TSoftClassPtr<UObject>, TSubclassOf<UUserWidget>> EntryWidgetForClass;
};
