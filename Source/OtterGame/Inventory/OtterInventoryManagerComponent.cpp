// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "OtterInventoryItemDefinition.h"
#include "OtterInventoryItemInstance.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterInventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Otter_Inventory_Message_StackChanged, "Otter.Inventory.Message.StackChanged");

//////////////////////////////////////////////////////////////////////
// FOtterInventoryEntry

FString FOtterInventoryEntry::GetDebugString() const
{
	TSubclassOf<UOtterInventoryItemDefinition> ItemDef;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

//////////////////////////////////////////////////////////////////////
// FOtterInventoryList

void FOtterInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FOtterInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.StackCount, /*NewCount=*/ 0);
		Stack.LastObservedCount = 0;
	}
}

void FOtterInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FOtterInventoryEntry& Stack = Entries[Index];
		BroadcastChangeMessage(Stack, /*OldCount=*/ 0, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FOtterInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FOtterInventoryEntry& Stack = Entries[Index];
		check(Stack.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Stack, /*OldCount=*/ Stack.LastObservedCount, /*NewCount=*/ Stack.StackCount);
		Stack.LastObservedCount = Stack.StackCount;
	}
}

void FOtterInventoryList::BroadcastChangeMessage(FOtterInventoryEntry& Entry, int32 OldCount, int32 NewCount)
{
	FOtterInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Otter_Inventory_Message_StackChanged, Message);
}

UOtterInventoryItemInstance* FOtterInventoryList::AddEntry(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UOtterInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
 	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());


	FOtterInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UOtterInventoryItemInstance>(OwnerComponent->GetOwner());  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDef);
	for (UOtterInventoryItemFragment* Fragment : GetDefault<UOtterInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}
	NewEntry.StackCount = StackCount;
	Result = NewEntry.Instance;

	//const UOtterInventoryItemDefinition* ItemCDO = GetDefault<UOtterInventoryItemDefinition>(ItemDef);
	MarkItemDirty(NewEntry);

	return Result;
}

void FOtterInventoryList::AddEntry(UOtterInventoryItemInstance* Instance)
{
	unimplemented();
}

void FOtterInventoryList::RemoveEntry(UOtterInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOtterInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

TArray<UOtterInventoryItemInstance*> FOtterInventoryList::GetAllItems() const
{
	TArray<UOtterInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FOtterInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

//////////////////////////////////////////////////////////////////////
// UOtterInventoryManagerComponent

UOtterInventoryManagerComponent::UOtterInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UOtterInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UOtterInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UOtterInventoryItemInstance* UOtterInventoryManagerComponent::AddItemDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UOtterInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDef, StackCount);
		
		if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
		{
			AddReplicatedSubObject(Result);
		}
	}
	return Result;
}

void UOtterInventoryManagerComponent::AddItemInstance(UOtterInventoryItemInstance* ItemInstance)
{
	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && ItemInstance)
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UOtterInventoryManagerComponent::RemoveItemInstance(UOtterInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveEntry(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UOtterInventoryItemInstance*> UOtterInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UOtterInventoryItemInstance* UOtterInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef) const
{
	for (const FOtterInventoryEntry& Entry : InventoryList.Entries)
	{
		UOtterInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UOtterInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FOtterInventoryEntry& Entry : InventoryList.Entries)
	{
		UOtterInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UOtterInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UOtterInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UOtterInventoryItemInstance* Instance = UOtterInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UOtterInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing UOtterInventoryItemInstance
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FOtterInventoryEntry& Entry : InventoryList.Entries)
		{
			UOtterInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

bool UOtterInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FOtterInventoryEntry& Entry : InventoryList.Entries)
	{
		UOtterInventoryItemInstance* Instance = Entry.Instance;

		if (Instance && IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

//////////////////////////////////////////////////////////////////////
//

// UCLASS(Abstract)
// class UOtterInventoryFilter : public UObject
// {
// public:
// 	virtual bool PassesFilter(UOtterInventoryItemInstance* Instance) const { return true; }
// };

// UCLASS()
// class UOtterInventoryFilter_HasTag : public UOtterInventoryFilter
// {
// public:
// 	virtual bool PassesFilter(UOtterInventoryItemInstance* Instance) const { return true; }
// };


