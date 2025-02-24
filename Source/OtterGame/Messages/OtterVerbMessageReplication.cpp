// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterVerbMessageReplication.h"

#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/OtterVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterVerbMessageReplication)

//////////////////////////////////////////////////////////////////////
// FOtterVerbMessageReplicationEntry

FString FOtterVerbMessageReplicationEntry::GetDebugString() const
{
	return Message.ToString();
}

//////////////////////////////////////////////////////////////////////
// FOtterVerbMessageReplication

void FOtterVerbMessageReplication::AddMessage(const FOtterVerbMessage& Message)
{
	FOtterVerbMessageReplicationEntry& NewStack = CurrentMessages.Emplace_GetRef(Message);
	MarkItemDirty(NewStack);
}

void FOtterVerbMessageReplication::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
// 	for (int32 Index : RemovedIndices)
// 	{
// 		const FGameplayTag Tag = CurrentMessages[Index].Tag;
// 		TagToCountMap.Remove(Tag);
// 	}
}

void FOtterVerbMessageReplication::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FOtterVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FOtterVerbMessageReplication::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FOtterVerbMessageReplicationEntry& Entry = CurrentMessages[Index];
		RebroadcastMessage(Entry.Message);
	}
}

void FOtterVerbMessageReplication::RebroadcastMessage(const FOtterVerbMessage& Message)
{
	check(Owner);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(Owner);
	MessageSystem.BroadcastMessage(Message.Verb, Message);
}

