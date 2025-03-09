// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/OtterCameraMode.h"
#include "OtterCameraMode_TopDown.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COREGAMEPLAYRUNTIME_API UOtterCameraMode_TopDown : public UOtterCameraMode
{
	GENERATED_BODY()
public:
	//~ULyraCameraMode interface
	virtual void UpdateView(float DeltaTime) override;
	//~End of ULyraCameraMode interface
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FRotator CameraRotation;
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	FVector CameraOffset;
};
