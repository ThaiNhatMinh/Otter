// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterBotCheats.h"
#include "Engine/World.h"
#include "GameFramework/CheatManagerDefines.h"
#include "GameModes/OtterBotCreationComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterBotCheats)

//////////////////////////////////////////////////////////////////////
// UOtterBotCheats

UOtterBotCheats::UOtterBotCheats()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UCheatManager::RegisterForOnCheatManagerCreated(FOnCheatManagerCreated::FDelegate::CreateLambda(
			[](UCheatManager* CheatManager)
			{
				CheatManager->AddCheatManagerExtension(NewObject<ThisClass>(CheatManager));
			}));
	}
#endif
}

void UOtterBotCheats::AddPlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UOtterBotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_AddBot();
	}
#endif	
}

void UOtterBotCheats::RemovePlayerBot()
{
#if WITH_SERVER_CODE && UE_WITH_CHEAT_MANAGER
	if (UOtterBotCreationComponent* BotComponent = GetBotComponent())
	{
		BotComponent->Cheat_RemoveBot();
	}
#endif	
}

UOtterBotCreationComponent* UOtterBotCheats::GetBotComponent() const
{
	if (UWorld* World = GetWorld())
	{
		if (AGameStateBase* GameState = World->GetGameState())
		{
			return GameState->FindComponentByClass<UOtterBotCreationComponent>();
		}
	}

	return nullptr;
}

