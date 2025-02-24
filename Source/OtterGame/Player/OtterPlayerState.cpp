// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPlayerState.h"

#include "AbilitySystem/Attributes/OtterCombatSet.h"
#include "AbilitySystem/Attributes/OtterHealthSet.h"
#include "AbilitySystem/OtterAbilitySet.h"
#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Character/OtterPawnData.h"
#include "Character/OtterPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/OtterExperienceManagerComponent.h"
//@TODO: Would like to isolate this a bit better to get the pawn data in here without this having to know about other stuff
#include "GameModes/OtterGameMode.h"
#include "OtterLogChannels.h"
#include "OtterPlayerController.h"
#include "Messages/OtterVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

const FName AOtterPlayerState::NAME_OtterAbilityReady("OtterAbilitiesReady");

AOtterPlayerState::AOtterPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EOtterPlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UOtterAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	MyTeamID = FGenericTeamId::NoTeam;
}

void AOtterPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOtterPlayerState::Reset()
{
	Super::Reset();
}

void AOtterPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AOtterPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AOtterPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
		case EOtterPlayerConnectionType::Player:
		case EOtterPlayerConnectionType::InactivePlayer:
			//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
			// (e.g., for long running servers where they might build up if lots of players cycle through)
			bDestroyDeactivatedPlayerState = true;
			break;
		default:
			bDestroyDeactivatedPlayerState = true;
			break;
	}
	
	SetPlayerConnectionType(EOtterPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AOtterPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EOtterPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EOtterPlayerConnectionType::Player);
	}
}

void AOtterPlayerState::OnExperienceLoaded(const UOtterExperienceDefinition* /*CurrentExperience*/)
{
	if (AOtterGameMode* OtterGameMode = GetWorld()->GetAuthGameMode<AOtterGameMode>())
	{
		if (const UOtterPawnData* NewPawnData = OtterGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogOtter, Error, TEXT("AOtterPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void AOtterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);

	DOREPLIFETIME(ThisClass, StatTags);	
}


AOtterPlayerController* AOtterPlayerState::GetOtterPlayerController() const
{
	return Cast<AOtterPlayerController>(GetOwner());
}

UAbilitySystemComponent* AOtterPlayerState::GetAbilitySystemComponent() const
{
	return GetOtterAbilitySystemComponent();
}

void AOtterPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UOtterExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UOtterExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnOtterExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void AOtterPlayerState::SetPawnData(const UOtterPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogOtter, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UOtterAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_OtterAbilityReady);
	
	ForceNetUpdate();
}

void AOtterPlayerState::OnRep_PawnData()
{
}

void AOtterPlayerState::SetPlayerConnectionType(EOtterPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}


void AOtterPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
	}
	else
	{
		UE_LOG(LogOtterTeams, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}

FGenericTeamId AOtterPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}


void AOtterPlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
}


void AOtterPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AOtterPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AOtterPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AOtterPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void AOtterPlayerState::ClientBroadcastMessage_Implementation(const FOtterVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

