// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterRuntimeOptions.h"

#include "UObject/Class.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterRuntimeOptions)

UOtterRuntimeOptions::UOtterRuntimeOptions()
{
	OptionCommandPrefix = TEXT("ro");
}

UOtterRuntimeOptions* UOtterRuntimeOptions::GetRuntimeOptions()
{
	return GetMutableDefault<UOtterRuntimeOptions>();
}

const UOtterRuntimeOptions& UOtterRuntimeOptions::Get()
{
	const UOtterRuntimeOptions& RuntimeOptions = *GetDefault<UOtterRuntimeOptions>();
	return RuntimeOptions;
}
