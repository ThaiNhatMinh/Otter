// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"

#include "OtterWeaponSpawner.generated.h"

namespace EEndPlayReason { enum Type : int; }

class APawn;
class UCapsuleComponent;
class UOtterInventoryItemDefinition;
class UOtterWeaponPickupDefinition;
class UObject;
class UPrimitiveComponent;
class UStaticMeshComponent;
struct FFrame;
struct FGameplayTag;
struct FHitResult;

UCLASS(Blueprintable,BlueprintType)
class OTTERGAME_API AOtterWeaponSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOtterWeaponSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

protected:
	//Data asset used to configure a Weapon Spawner
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Otter|WeaponPickup")
	TObjectPtr<UOtterWeaponPickupDefinition> WeaponDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_WeaponAvailability, Category = "Otter|WeaponPickup")
	bool bIsWeaponAvailable;

	//The amount of time between weapon pickup and weapon spawning in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|WeaponPickup")
	float CoolDownTime;

	//Delay between when the weapon is made available and when we check for a pawn standing in the spawner. Used to give the bIsWeaponAvailable OnRep time to fire and play FX. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|WeaponPickup")
	float CheckExistingOverlapDelay;

	//Used to drive weapon respawn time indicators 0-1
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Otter|WeaponPickup")
	float CoolDownPercentage;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|WeaponPickup")
	TObjectPtr<UCapsuleComponent> CollisionVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Otter|WeaponPickup")
	TObjectPtr<UStaticMeshComponent> PadMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Otter|WeaponPickup")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Otter|WeaponPickup")
	float WeaponMeshRotationSpeed;

	FTimerHandle CoolDownTimerHandle;

	FTimerHandle CheckOverlapsDelayTimerHandle;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	//Check for pawns standing on pad when the weapon is spawned. 
	void CheckForExistingOverlaps();

	UFUNCTION(BlueprintNativeEvent)
	void AttemptPickUpWeapon(APawn* Pawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "Otter|WeaponPickup")
	bool GiveWeapon(TSubclassOf<UOtterInventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);

	void StartCoolDown();

	UFUNCTION(BlueprintCallable, Category = "Otter|WeaponPickup")
	void ResetCoolDown();

	UFUNCTION()
	void OnCoolDownTimerComplete();

	void SetWeaponPickupVisibility(bool bShouldBeVisible);

	UFUNCTION(BlueprintNativeEvent, Category = "Otter|WeaponPickup")
	void PlayPickupEffects();

	UFUNCTION(BlueprintNativeEvent, Category = "Otter|WeaponPickup")
	void PlayRespawnEffects();

	UFUNCTION()
	void OnRep_WeaponAvailability();

	/** Searches an item definition type for a matching stat and returns the value, or 0 if not found */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Otter|WeaponPickup")
	static int32 GetDefaultStatFromItemDef(const TSubclassOf<UOtterInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag);
};
