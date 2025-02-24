// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterIndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterIndicatorManagerComponent)

UOtterIndicatorManagerComponent::UOtterIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

/*static*/ UOtterIndicatorManagerComponent* UOtterIndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UOtterIndicatorManagerComponent>();
	}

	return nullptr;
}

void UOtterIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UOtterIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);
	
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
