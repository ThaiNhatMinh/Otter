// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPlayerCharacter.h"

#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Camera/OtterCameraComponent.h"
#include "Character/OtterHealthComponent.h"
#include "Character/OtterPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "OtterCharacterMovementComponent.h"
#include "OtterLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "Player/OtterPlayerController.h"
#include "Player/OtterPlayerState.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPlayerCharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

AOtterPlayerCharacter::AOtterPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnExtComponent = CreateDefaultSubobject<UOtterPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<UOtterHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<UOtterCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
}

void AOtterPlayerCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AOtterPlayerCharacter::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();
}

AOtterPlayerController* AOtterPlayerCharacter::GetOtterPlayerController() const
{
	return CastChecked<AOtterPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AOtterPlayerState* AOtterPlayerCharacter::GetOtterPlayerState() const
{
	return CastChecked<AOtterPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UAbilitySystemComponent* AOtterPlayerCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetOtterAbilitySystemComponent();
}

void AOtterPlayerCharacter::OnAbilitySystemInitialized()
{
	UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent();
	check(OtterASC);

	HealthComponent->InitializeWithAbilitySystem(OtterASC);

	InitializeGameplayTags();
}

void AOtterPlayerCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void AOtterPlayerCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void AOtterPlayerCharacter::UnPossessed()
{
	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
}

void AOtterPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void AOtterPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void AOtterPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AOtterPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void AOtterPlayerCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AOtterPlayerCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void AOtterPlayerCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UOtterCharacterMovementComponent* OtterMoveComp = CastChecked<UOtterCharacterMovementComponent>(GetCharacterMovement());
	OtterMoveComp->StopMovementImmediately();
	OtterMoveComp->DisableMovement();
}

void AOtterPlayerCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void AOtterPlayerCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		if (OtterASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}
