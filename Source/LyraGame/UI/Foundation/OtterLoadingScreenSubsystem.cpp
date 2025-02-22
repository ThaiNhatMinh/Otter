// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Foundation/OtterLoadingScreenSubsystem.h"

#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterLoadingScreenSubsystem)

class UUserWidget;

//////////////////////////////////////////////////////////////////////
// UOtterLoadingScreenSubsystem

UOtterLoadingScreenSubsystem::UOtterLoadingScreenSubsystem()
{
}

void UOtterLoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> UOtterLoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}

