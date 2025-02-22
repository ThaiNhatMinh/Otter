// Copyright Epic Games, Inc. All Rights Reserved.

#include "MessageProcessors/ElimChainProcessor.h"

#include "GameFramework/PlayerState.h"
#include "Messages/OtterVerbMessage.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ElimChainProcessor)

namespace ElimChain
{
	UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Otter_Elimination_Message, "Otter.Elimination.Message");
}

void UElimChainProcessor::StartListening()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	AddListenerHandle(MessageSubsystem.RegisterListener(ElimChain::TAG_Otter_Elimination_Message, this, &ThisClass::OnEliminationMessage));
}

void UElimChainProcessor::OnEliminationMessage(FGameplayTag Channel, const FOtterVerbMessage& Payload)
{
	// Track elimination chains for the attacker (except for self-eliminations)
	if (Payload.Instigator != Payload.Target)
	{
		if (APlayerState* InstigatorPS = Cast<APlayerState>(Payload.Instigator))
		{
			const double CurrentTime = GetServerTime();

			FPlayerElimChainInfo& History = PlayerChainHistory.FindOrAdd(InstigatorPS);
			const bool bStreakReset = (History.LastEliminationTime == 0.0) || (History.LastEliminationTime + ChainTimeLimit < CurrentTime);

			History.LastEliminationTime = CurrentTime;
			if (bStreakReset)
			{
				History.ChainCounter = 1;
			}
			else
			{
				++History.ChainCounter;

				if (FGameplayTag* pTag = ElimChainTags.Find(History.ChainCounter))
				{
					FOtterVerbMessage ElimChainMessage;
					ElimChainMessage.Verb = *pTag;
					ElimChainMessage.Instigator = InstigatorPS;
					ElimChainMessage.InstigatorTags = Payload.InstigatorTags;
					ElimChainMessage.ContextTags = Payload.ContextTags;
					ElimChainMessage.Magnitude = History.ChainCounter;
					
					UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
					MessageSubsystem.BroadcastMessage(ElimChainMessage.Verb, ElimChainMessage);
				}
			}
		}
	}
}

