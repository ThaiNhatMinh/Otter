// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterCharacter.h"

#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Camera/OtterCameraComponent.h"
#include "Character/OtterHealthComponent.h"
#include "Character/OtterPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "OtterCharacterMovementComponent.h"
#include "OtterGameplayTags.h"
#include "OtterLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "Player/OtterPlayerController.h"
#include "Player/OtterPlayerState.h"
#include "System/OtterSignificanceManager.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

static FName NAME_OtterCharacterCollisionProfile_Capsule(TEXT("OtterPawnCapsule"));
static FName NAME_OtterCharacterCollisionProfile_Mesh(TEXT("OtterPawnMesh"));

AOtterCharacter::AOtterCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOtterCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetNetCullDistanceSquared(900000000.0f);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_OtterCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_OtterCharacterCollisionProfile_Mesh);

	UOtterCharacterMovementComponent* OtterMoveComp = CastChecked<UOtterCharacterMovementComponent>(GetCharacterMovement());
	OtterMoveComp->GravityScale = 1.0f;
	OtterMoveComp->MaxAcceleration = 2400.0f;
	OtterMoveComp->BrakingFrictionFactor = 1.0f;
	OtterMoveComp->BrakingFriction = 6.0f;
	OtterMoveComp->GroundFriction = 8.0f;
	OtterMoveComp->BrakingDecelerationWalking = 1400.0f;
	OtterMoveComp->bUseControllerDesiredRotation = false;
	OtterMoveComp->bOrientRotationToMovement = false;
	OtterMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	OtterMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	OtterMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	OtterMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	OtterMoveComp->SetCrouchedHalfHeight(65.0f);

	PawnExtComponent = CreateDefaultSubobject<UOtterPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	HealthComponent = CreateDefaultSubobject<UOtterHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<UOtterCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void AOtterCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOtterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UOtterSignificanceManager* SignificanceManager = USignificanceManager::Get<UOtterSignificanceManager>(World))
		{
//@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
		}
	}
}

void AOtterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (UOtterSignificanceManager* SignificanceManager = USignificanceManager::Get<UOtterSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

void AOtterCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AOtterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID)
}

void AOtterCharacter::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();
}

AOtterPlayerController* AOtterCharacter::GetOtterPlayerController() const
{
	return CastChecked<AOtterPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AOtterPlayerState* AOtterCharacter::GetOtterPlayerState() const
{
	return CastChecked<AOtterPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UOtterAbilitySystemComponent* AOtterCharacter::GetOtterAbilitySystemComponent() const
{
	return Cast<UOtterAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AOtterCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetOtterAbilitySystemComponent();
}

void AOtterCharacter::OnAbilitySystemInitialized()
{
	UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent();
	check(OtterASC);

	HealthComponent->InitializeWithAbilitySystem(OtterASC);

	InitializeGameplayTags();
}

void AOtterCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void AOtterCharacter::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void AOtterCharacter::UnPossessed()
{
	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
}

void AOtterCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void AOtterCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void AOtterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}

void AOtterCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : OtterGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				OtterASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : OtterGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				OtterASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UOtterCharacterMovementComponent* OtterMoveComp = CastChecked<UOtterCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(OtterMoveComp->MovementMode, OtterMoveComp->CustomMovementMode, true);
	}
}

void AOtterCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		OtterASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AOtterCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		return OtterASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AOtterCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		return OtterASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AOtterCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		return OtterASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AOtterCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void AOtterCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AOtterCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void AOtterCharacter::DisableMovementAndCollision()
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

void AOtterCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void AOtterCharacter::UninitAndDestroy()
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

void AOtterCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UOtterCharacterMovementComponent* OtterMoveComp = CastChecked<UOtterCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(OtterMoveComp->MovementMode, OtterMoveComp->CustomMovementMode, true);
}

void AOtterCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = OtterGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = OtterGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			OtterASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void AOtterCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		MyTeamID = NewTeamID;
	}
	else
	{
		UE_LOG(LogOtterTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
	}
}

FGenericTeamId AOtterCharacter::GetGenericTeamId() const
{
	return MyTeamID;
}

void AOtterCharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
}
