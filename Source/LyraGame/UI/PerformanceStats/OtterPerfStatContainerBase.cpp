// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPerfStatContainerBase.h"

#include "Blueprint/WidgetTree.h"
#include "OtterPerfStatWidgetBase.h"
#include "Settings/OtterSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPerfStatContainerBase)

//////////////////////////////////////////////////////////////////////
// UOtterPerfStatsContainerBase

UOtterPerfStatContainerBase::UOtterPerfStatContainerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOtterPerfStatContainerBase::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateVisibilityOfChildren();

	UOtterSettingsLocal::Get()->OnPerfStatDisplayStateChanged().AddUObject(this, &ThisClass::UpdateVisibilityOfChildren);
}

void UOtterPerfStatContainerBase::NativeDestruct()
{
	UOtterSettingsLocal::Get()->OnPerfStatDisplayStateChanged().RemoveAll(this);

	Super::NativeDestruct();
}

void UOtterPerfStatContainerBase::UpdateVisibilityOfChildren()
{
	UOtterSettingsLocal* UserSettings = UOtterSettingsLocal::Get();

	const bool bShowTextWidgets = (StatDisplayModeFilter == EOtterStatDisplayMode::TextOnly) || (StatDisplayModeFilter == EOtterStatDisplayMode::TextAndGraph);
	const bool bShowGraphWidgets = (StatDisplayModeFilter == EOtterStatDisplayMode::GraphOnly) || (StatDisplayModeFilter == EOtterStatDisplayMode::TextAndGraph);
	
	check(WidgetTree);
	WidgetTree->ForEachWidget([&](UWidget* Widget)
	{
		if (UOtterPerfStatWidgetBase* TypedWidget = Cast<UOtterPerfStatWidgetBase>(Widget))
		{
			const EOtterStatDisplayMode SettingMode = UserSettings->GetPerfStatDisplayState(TypedWidget->GetStatToDisplay());

			bool bShowWidget = false;
			switch (SettingMode)
			{
			case EOtterStatDisplayMode::Hidden:
				bShowWidget = false;
				break;
			case EOtterStatDisplayMode::TextOnly:
				bShowWidget = bShowTextWidgets;
				break;
			case EOtterStatDisplayMode::GraphOnly:
				bShowWidget = bShowGraphWidgets;
				break;
			case EOtterStatDisplayMode::TextAndGraph:
				bShowWidget = bShowTextWidgets || bShowGraphWidgets;
				break;
			}

			TypedWidget->SetVisibility(bShowWidget ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
		}
	});
}

