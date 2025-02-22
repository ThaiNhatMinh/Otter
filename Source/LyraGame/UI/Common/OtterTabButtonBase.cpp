// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterTabButtonBase.h"

#include "CommonLazyImage.h"
#include "UI/Common/OtterTabListWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterTabButtonBase)

class UObject;
struct FSlateBrush;

void UOtterTabButtonBase::SetIconFromLazyObject(TSoftObjectPtr<UObject> LazyObject)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrushFromLazyDisplayAsset(LazyObject);
	}
}

void UOtterTabButtonBase::SetIconBrush(const FSlateBrush& Brush)
{
	if (LazyImage_Icon)
	{
		LazyImage_Icon->SetBrush(Brush);
	}
}

void UOtterTabButtonBase::SetTabLabelInfo_Implementation(const FOtterTabDescriptor& TabLabelInfo)
{
	SetButtonText(TabLabelInfo.TabText);
	SetIconBrush(TabLabelInfo.IconBrush);
}

