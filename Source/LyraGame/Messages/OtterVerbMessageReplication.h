// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "OtterVerbMessage.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "OtterVerbMessageReplication.generated.h"

class UObject;
struct FOtterVerbMessageReplication;
struct FNetDeltaSerializeInfo;

/**
 * Represents one verb message
 */
USTRUCT(BlueprintType)
struct FOtterVerbMessageReplicationEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FOtterVerbMessageReplicationEntry()
	{}

	FOtterVerbMessageReplicationEntry(const FOtterVerbMessage& InMessage)
		: Message(InMessage)
	{
	}

	FString GetDebugString() const;

private:
	friend FOtterVerbMessageReplication;

	UPROPERTY()
	FOtterVerbMessage Message;
};

/** Container of verb messages to replicate */
USTRUCT(BlueprintType)
struct FOtterVerbMessageReplication : public FFastArraySerializer
{
	GENERATED_BODY()

	FOtterVerbMessageReplication()
	{
	}

public:
	void SetOwner(UObject* InOwner) { Owner = InOwner; }

	// Broadcasts a message from server to clients
	void AddMessage(const FOtterVerbMessage& Message);

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOtterVerbMessageReplicationEntry, FOtterVerbMessageReplication>(CurrentMessages, DeltaParms, *this);
	}

private:
	void RebroadcastMessage(const FOtterVerbMessage& Message);

private:
	// Replicated list of gameplay tag stacks
	UPROPERTY()
	TArray<FOtterVerbMessageReplicationEntry> CurrentMessages;
	
	// Owner (for a route to a world)
	UPROPERTY()
	TObjectPtr<UObject> Owner = nullptr;
};

template<>
struct TStructOpsTypeTraits<FOtterVerbMessageReplication> : public TStructOpsTypeTraitsBase2<FOtterVerbMessageReplication>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
