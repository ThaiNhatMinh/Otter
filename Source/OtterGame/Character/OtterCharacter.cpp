#include "OtterCharacter.h"
#include "OtterCharacterMovementComponent.h"
#include "OtterGameplayTags.h"
#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "System/OtterSignificanceManager.h"
#include "Net/UnrealNetwork.h"
#include "OtterLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterCharacter)


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

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}


void AOtterCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		if (auto SignificanceManager = USignificanceManager::Get<UOtterSignificanceManager>(World))
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
		if (auto SignificanceManager = USignificanceManager::Get<UOtterSignificanceManager>(World))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}
}

UOtterAbilitySystemComponent* AOtterCharacter::GetOtterAbilitySystemComponent() const
{
	return Cast<UOtterAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AOtterCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UOtterAbilitySystemComponent* OtterASC = GetOtterAbilitySystemComponent())
	{
		for (const auto& TagMapping : OtterGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				OtterASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const auto& TagMapping : OtterGameplayTags::CustomMovementModeTagMap)
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

void AOtterCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID)
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