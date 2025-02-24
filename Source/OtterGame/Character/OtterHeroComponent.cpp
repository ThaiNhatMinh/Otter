// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "OtterLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "Player/OtterPlayerController.h"
#include "Player/OtterPlayerState.h"
#include "Player/OtterLocalPlayer.h"
#include "Character/OtterPawnExtensionComponent.h"
#include "Character/OtterPawnData.h"
#include "Character/OtterCharacter.h"
#include "AbilitySystem/OtterAbilitySystemComponent.h"
#include "Input/OtterInputConfig.h"
#include "Input/OtterInputComponent.h"
#include "Camera/OtterCameraComponent.h"
#include "OtterGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/OtterCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace OtterHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UOtterHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UOtterHeroComponent::NAME_ActorFeatureName("Hero");

UOtterHeroComponent::UOtterHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UOtterHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogOtter, Error, TEXT("[UOtterHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("OtterHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("OtterHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UOtterHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == OtterGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == OtterGameplayTags::InitState_Spawned && DesiredState == OtterGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AOtterPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			AOtterPlayerController* OtterPC = GetController<AOtterPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !OtterPC || !OtterPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == OtterGameplayTags::InitState_DataAvailable && DesiredState == OtterGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		AOtterPlayerState* OtterPS = GetPlayerState<AOtterPlayerState>();

		return OtterPS && Manager->HasFeatureReachedInitState(Pawn, UOtterPawnExtensionComponent::NAME_ActorFeatureName, OtterGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == OtterGameplayTags::InitState_DataInitialized && DesiredState == OtterGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UOtterHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == OtterGameplayTags::InitState_DataAvailable && DesiredState == OtterGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AOtterPlayerState* OtterPS = GetPlayerState<AOtterPlayerState>();
		if (!ensure(Pawn && OtterPS))
		{
			return;
		}

		const UOtterPawnData* PawnData = nullptr;

		if (UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UOtterPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			PawnExtComp->InitializeAbilitySystem(OtterPS->GetOtterAbilitySystemComponent(), OtterPS);
		}

		if (AOtterPlayerController* OtterPC = GetController<AOtterPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		if (PawnData)
		{
			if (UOtterCameraComponent* CameraComponent = UOtterCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UOtterHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UOtterPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == OtterGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UOtterHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { OtterGameplayTags::InitState_Spawned, OtterGameplayTags::InitState_DataAvailable, OtterGameplayTags::InitState_DataInitialized, OtterGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UOtterHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UOtterPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(OtterGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UOtterHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UOtterHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UOtterLocalPlayer* LP = Cast<UOtterLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UOtterPawnData* PawnData = PawnExtComp->GetPawnData<UOtterPawnData>())
		{
			if (const UOtterInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Otter Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UOtterInputComponent or modify this component accordingly.
				UOtterInputComponent* OtterIC = Cast<UOtterInputComponent>(PlayerInputComponent);
				if (ensureMsgf(OtterIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOtterInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					OtterIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					OtterIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					OtterIC->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					OtterIC->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					OtterIC->BindNativeAction(InputConfig, OtterGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}
 
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UOtterHeroComponent::AddAdditionalInputConfig(const UOtterInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	
	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UOtterInputComponent* OtterIC = Pawn->FindComponentByClass<UOtterInputComponent>();
		if (ensureMsgf(OtterIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOtterInputComponent or a subclass of it.")))
		{
			OtterIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UOtterHeroComponent::RemoveAdditionalInputConfig(const UOtterInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UOtterHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UOtterHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UOtterAbilitySystemComponent* OtterASC = PawnExtComp->GetOtterAbilitySystemComponent())
			{
				OtterASC->AbilityInputTagPressed(InputTag);
			}
		}	
	}
}

void UOtterHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	if (const UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (UOtterAbilitySystemComponent* OtterASC = PawnExtComp->GetOtterAbilitySystemComponent())
		{
			OtterASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UOtterHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UOtterHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UOtterHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * OtterHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * OtterHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

TSubclassOf<UOtterCameraMode> UOtterHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UOtterPawnExtensionComponent* PawnExtComp = UOtterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UOtterPawnData* PawnData = PawnExtComp->GetPawnData<UOtterPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

