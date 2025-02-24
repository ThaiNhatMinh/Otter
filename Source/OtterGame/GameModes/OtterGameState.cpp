// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterGameState.h"

#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameModes/OtterExperienceManagerComponent.h"
#include "Messages/OtterVerbMessage.h"
#include "Player/OtterPlayerState.h"
#include "OtterLogChannels.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterGameState)

class APlayerState;
class FLifetimeProperty;

extern ENGINE_API float GAverageFPS;


AOtterGameState::AOtterGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UOtterAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ExperienceManagerComponent = CreateDefaultSubobject<UOtterExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));

	ServerFPS = 0.0f;
}

void AOtterGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AOtterGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* AOtterGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOtterGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AOtterGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AOtterGameState::RemovePlayerState(APlayerState* PlayerState)
{
	//@TODO: This isn't getting called right now (only the 'rich' AGameMode uses it, not AGameModeBase)
	// Need to at least comment the engine code, and possibly move things around
	Super::RemovePlayerState(PlayerState);
}

void AOtterGameState::SeamlessTravelTransitionCheckpoint(bool bToTransitionMap)
{
	// Remove inactive and bots
	for (int32 i = PlayerArray.Num() - 1; i >= 0; i--)
	{
		APlayerState* PlayerState = PlayerArray[i];
		if (PlayerState && (PlayerState->IsABot() || PlayerState->IsInactive()))
		{
			RemovePlayerState(PlayerState);
		}
	}
}

void AOtterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerFPS);
	DOREPLIFETIME_CONDITION(ThisClass, RecorderPlayerState, COND_ReplayOnly);
}

void AOtterGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() == ROLE_Authority)
	{
		ServerFPS = GAverageFPS;
	}
}

void AOtterGameState::MulticastMessageToClients_Implementation(const FOtterVerbMessage Message)
{
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

void AOtterGameState::MulticastReliableMessageToClients_Implementation(const FOtterVerbMessage Message)
{
	MulticastMessageToClients_Implementation(Message);
}

float AOtterGameState::GetServerFPS() const
{
	return ServerFPS;
}

void AOtterGameState::SetRecorderPlayerState(APlayerState* NewPlayerState)
{
	if (RecorderPlayerState == nullptr)
	{
		// Set it and call the rep callback so it can do any record-time setup
		RecorderPlayerState = NewPlayerState;
		OnRep_RecorderPlayerState();
	}
	else
	{
		UE_LOG(LogOtter, Warning, TEXT("SetRecorderPlayerState was called on %s but should only be called once per game on the primary user"), *GetName());
	}
}

APlayerState* AOtterGameState::GetRecorderPlayerState() const
{
	// TODO: Maybe auto select it if null?

	return RecorderPlayerState;
}

void AOtterGameState::OnRep_RecorderPlayerState()
{
	OnRecorderPlayerStateChangedEvent.Broadcast(RecorderPlayerState);
}