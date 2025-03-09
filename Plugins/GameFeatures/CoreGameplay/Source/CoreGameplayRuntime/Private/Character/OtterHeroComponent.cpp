// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OtterHeroComponent.h"
#include "Input/OtterInputConfig.h"
#include "Input/OtterInputComponent.h"
#include "OtterGameplayTags.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/OtterCameraComponent.h"

namespace OtterHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

UOtterHeroComponent::UOtterHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UOtterHeroComponent::InitializePlayerInput(const UOtterInputConfig *InputConfig, UOtterInputComponent *InputComponent)
{
    InputComponent->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
    InputComponent->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
    InputComponent->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
}

void UOtterHeroComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto Pawn = GetPawn<APawn>();
	auto PlayerController = Pawn ? Pawn->GetController<APlayerController>() : nullptr;
	if (!PlayerController)
		return;
	FVector WorldLocation;
	FVector WorldDirection;
	if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		return;
	float T;
	auto ActorLocation = Pawn->GetActorLocation();
	FVector Intersection;
	if (!UKismetMathLibrary::LinePlaneIntersection(WorldLocation, WorldLocation + WorldDirection * 10000.0f, FPlane(Pawn->GetActorLocation(), FVector::UpVector), T, Intersection))
		return;
	Intersection.Z = ActorLocation.Z;
	PlayerController->SetControlRotation((Intersection - ActorLocation).Rotation());
	//DrawDebugLine(GetWorld(), ActorLocation, ActorLocation + Pawn->GetActorForwardVector() * 100.0f, FColor::Green, false, 0);
	//DrawDebugLine(GetWorld(), ActorLocation, ActorLocation + PlayerController->GetControlRotation().Vector() * 100.0f, FColor::Red, false, 0);
}

void UOtterHeroComponent::Input_Move(const FInputActionValue &InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (!Controller)
        return;
    const FVector2D Value = InputActionValue.Get<FVector2D>();
	auto CameraComponent = Pawn->FindComponentByClass<UOtterCameraComponent>();
	if (!CameraComponent)
		return;

    const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

    if (Value.X != 0.0f)
    {
		const FVector MovementDirection = CameraComponent->GetRightVector();
        Pawn->AddMovementInput(MovementDirection, Value.X);
    }

    if (Value.Y != 0.0f)
    {
		const FVector MovementDirection = CameraComponent->GetForwardVector();
		Pawn->AddMovementInput(MovementDirection, Value.Y);
    }
}

void UOtterHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	
	if (!Pawn)
	{
		return;
	}
	const UWorld* World = GetWorld();
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * OtterHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UOtterHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * OtterHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * OtterHero::LookPitchRate * World->GetDeltaSeconds());
	}
}