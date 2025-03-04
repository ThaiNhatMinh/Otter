// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularCharacter.h"
#include "Teams/OtterTeamAgentInterface.h"

#include "OtterCharacter.generated.h"

class UAbilitySystemComponent;
class UOtterAbilitySystemComponent;

UCLASS(Abstract)
class OTTERGAME_API AOtterCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface, public IOtterTeamAgentInterface
{
	GENERATED_BODY()

public:
	AOtterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface
	
	//~IOtterTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~End of IOtterTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "Otter|Character")
	UOtterAbilitySystemComponent* GetOtterAbilitySystemComponent() const;

	void InitializeGameplayTags();
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	//~IGameplayTagAssetInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return nullptr; };
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~End of IGameplayTagAssetInterface interface

protected:
	UFUNCTION()
	virtual void OnRep_MyTeamID(FGenericTeamId OldTeamID);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;
};
