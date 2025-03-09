// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/OtterCameraMode_TopDown.h"

void UOtterCameraMode_TopDown::UpdateView(float DeltaTime)
{
    FVector PivotLocation = GetPivotLocation();

    View.Location = PivotLocation + CameraOffset;
    auto Direction = PivotLocation - View.Location;
    Direction.Normalize();
    View.Rotation = Direction.ToOrientationRotator();
    View.ControlRotation = View.Rotation;
    View.FieldOfView = FieldOfView;
}