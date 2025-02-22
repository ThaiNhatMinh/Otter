// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ExternalRpcRegistrationComponent.h"
#include "Serialization/JsonSerializer.h"

#include "Dom/JsonObject.h"
#include "OtterGameplayRpcRegistrationComponent.generated.h"


UCLASS()
class LYRAGAME_API UOtterGameplayRpcRegistrationComponent : public UExternalRpcRegistrationComponent
{
	GENERATED_BODY()
protected:
	static UOtterGameplayRpcRegistrationComponent* ObjectInstance;

public:
	static UOtterGameplayRpcRegistrationComponent* GetInstance();


#if WITH_RPC_REGISTRY
	/**
	* Basic function that converts any request into a proper Json body.
	*/
	TSharedPtr<FJsonObject> GetJsonObjectFromRequestBody(TArray<uint8> InRequestBody);

	virtual void DeregisterHttpCallbacks() override;


// These are RPCs that should always be enabled, no matter what state the game is in. They 

	virtual void RegisterAlwaysOnHttpCallbacks() override;
	/** 
	* This is an example RPC that shows how to interact with request bodies and gather passed in values for use in game code.
	*/
	bool HttpExecuteCheatCommand(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);


// These are RPCs that should only be enabled while we are in the frontend.

	virtual void RegisterFrontendHttpCallbacks();
	//bool HttpSetMatchType(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

// These are RPCs that should only be enabled while we are in a match

	virtual void RegisterInMatchHttpCallbacks();
	/**
	 * This is an example RPC that shows how you can use requests to cause in-game input.
	 */
	bool HttpFireOnceCommand(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	/**
	 * This is an example RPC that shows how you can put together a response for consumption.
	 */
	bool HttpGetPlayerVitalsCommand(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

#endif

};