// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterReplicationGraphSettings.h"
#include "Misc/App.h"
#include "System/OtterReplicationGraph.h"

UOtterReplicationGraphSettings::UOtterReplicationGraphSettings()
{
	CategoryName = TEXT("Game");
	DefaultReplicationGraphClass = UOtterReplicationGraph::StaticClass();
}