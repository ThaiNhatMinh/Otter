// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystem/OtterAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "OtterEquipmentManagerComponent.generated.h"

class UActorComponent;
class UOtterAbilitySystemComponent;
class UOtterEquipmentDefinition;
class UOtterEquipmentInstance;
class UOtterEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FOtterEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FOtterAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FOtterAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FOtterEquipmentList;
	friend UOtterEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UOtterEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UOtterEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FOtterAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FOtterEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FOtterEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FOtterEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOtterAppliedEquipmentEntry, FOtterEquipmentList>(Entries, DeltaParms, *this);
	}

	UOtterEquipmentInstance* AddEntry(TSubclassOf<UOtterEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UOtterEquipmentInstance* Instance);

private:
	UOtterAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UOtterEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FOtterAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FOtterEquipmentList> : public TStructOpsTypeTraitsBase2<FOtterEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};










/**
 * Manages equipment applied to a pawn
 */
UCLASS(BlueprintType, Const)
class OTTERGAME_API UOtterEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UOtterEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UOtterEquipmentInstance* EquipItem(TSubclassOf<UOtterEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UOtterEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UOtterEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UOtterEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UOtterEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UOtterEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FOtterEquipmentList EquipmentList;
};
