// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "OtterInventoryManagerComponent.generated.h"

class UOtterInventoryItemDefinition;
class UOtterInventoryItemInstance;
class UOtterInventoryManagerComponent;
class UObject;
struct FFrame;
struct FOtterInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FOtterInventoryChangeMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UOtterInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};

/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FOtterInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FOtterInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FOtterInventoryList;
	friend UOtterInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UOtterInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** List of inventory items */
USTRUCT(BlueprintType)
struct FOtterInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FOtterInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FOtterInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UOtterInventoryItemInstance*> GetAllItems() const;

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOtterInventoryEntry, FOtterInventoryList>(Entries, DeltaParms, *this);
	}

	UOtterInventoryItemInstance* AddEntry(TSubclassOf<UOtterInventoryItemDefinition> ItemClass, int32 StackCount);
	void AddEntry(UOtterInventoryItemInstance* Instance);

	void RemoveEntry(UOtterInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FOtterInventoryEntry& Entry, int32 OldCount, int32 NewCount);

private:
	friend UOtterInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
	TArray<FOtterInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FOtterInventoryList> : public TStructOpsTypeTraitsBase2<FOtterInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages an inventory
 */
UCLASS(BlueprintType)
class LYRAGAME_API UOtterInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOtterInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	bool CanAddItemDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UOtterInventoryItemInstance* AddItemDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddItemInstance(UOtterInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItemInstance(UOtterInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UOtterInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UOtterInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FOtterInventoryList InventoryList;
};
