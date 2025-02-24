// Copyright Epic Games, Inc. All Rights Reserved.

#include "OtterPerformanceStatSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/OtterGameState.h"
#include "Performance/OtterPerformanceStatTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OtterPerformanceStatSubsystem)

class FSubsystemCollectionBase;

//////////////////////////////////////////////////////////////////////
// FOtterPerformanceStatCache

void FOtterPerformanceStatCache::StartCharting()
{
}

void FOtterPerformanceStatCache::ProcessFrame(const FFrameData& FrameData)
{
	CachedData = FrameData;
	CachedServerFPS = 0.0f;
	CachedPingMS = 0.0f;
	CachedPacketLossIncomingPercent = 0.0f;
	CachedPacketLossOutgoingPercent = 0.0f;
	CachedPacketRateIncoming = 0.0f;
	CachedPacketRateOutgoing = 0.0f;
	CachedPacketSizeIncoming = 0.0f;
	CachedPacketSizeOutgoing = 0.0f;

	if (UWorld* World = MySubsystem->GetGameInstance()->GetWorld())
	{
		if (const AOtterGameState* GameState = World->GetGameState<AOtterGameState>())
		{
			CachedServerFPS = GameState->GetServerFPS();
		}

		if (APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(World))
		{
			if (APlayerState* PS = LocalPC->GetPlayerState<APlayerState>())
			{
				CachedPingMS = PS->GetPingInMilliseconds();
			}

			if (UNetConnection* NetConnection = LocalPC->GetNetConnection())
			{
				const UNetConnection::FNetConnectionPacketLoss& InLoss = NetConnection->GetInLossPercentage();
				CachedPacketLossIncomingPercent = InLoss.GetAvgLossPercentage();
				const UNetConnection::FNetConnectionPacketLoss& OutLoss = NetConnection->GetOutLossPercentage();
				CachedPacketLossOutgoingPercent = OutLoss.GetAvgLossPercentage();

				CachedPacketRateIncoming = NetConnection->InPacketsPerSecond;
				CachedPacketRateOutgoing = NetConnection->OutPacketsPerSecond;

				CachedPacketSizeIncoming = (NetConnection->InPacketsPerSecond != 0) ? (NetConnection->InBytesPerSecond / (float)NetConnection->InPacketsPerSecond) : 0.0f;
				CachedPacketSizeOutgoing = (NetConnection->OutPacketsPerSecond != 0) ? (NetConnection->OutBytesPerSecond / (float)NetConnection->OutPacketsPerSecond) : 0.0f;
			}
		}
	}
}

void FOtterPerformanceStatCache::StopCharting()
{
}

double FOtterPerformanceStatCache::GetCachedStat(EOtterDisplayablePerformanceStat Stat) const
{
	static_assert((int32)EOtterDisplayablePerformanceStat::Count == 15, "Need to update this function to deal with new performance stats");
	switch (Stat)
	{
	case EOtterDisplayablePerformanceStat::ClientFPS:
		return (CachedData.TrueDeltaSeconds != 0.0) ? (1.0 / CachedData.TrueDeltaSeconds) : 0.0;
	case EOtterDisplayablePerformanceStat::ServerFPS:
		return CachedServerFPS;
	case EOtterDisplayablePerformanceStat::IdleTime:
		return CachedData.IdleSeconds;
	case EOtterDisplayablePerformanceStat::FrameTime:
		return CachedData.TrueDeltaSeconds;
	case EOtterDisplayablePerformanceStat::FrameTime_GameThread:
		return CachedData.GameThreadTimeSeconds;
	case EOtterDisplayablePerformanceStat::FrameTime_RenderThread:
		return CachedData.RenderThreadTimeSeconds;
	case EOtterDisplayablePerformanceStat::FrameTime_RHIThread:
		return CachedData.RHIThreadTimeSeconds;
	case EOtterDisplayablePerformanceStat::FrameTime_GPU:
		return CachedData.GPUTimeSeconds;
	case EOtterDisplayablePerformanceStat::Ping:
		return CachedPingMS;
	case EOtterDisplayablePerformanceStat::PacketLoss_Incoming:
		return CachedPacketLossIncomingPercent;
	case EOtterDisplayablePerformanceStat::PacketLoss_Outgoing:
		return CachedPacketLossOutgoingPercent;
	case EOtterDisplayablePerformanceStat::PacketRate_Incoming:
		return CachedPacketRateIncoming;
	case EOtterDisplayablePerformanceStat::PacketRate_Outgoing:
		return CachedPacketRateOutgoing;
	case EOtterDisplayablePerformanceStat::PacketSize_Incoming:
		return CachedPacketSizeIncoming;
	case EOtterDisplayablePerformanceStat::PacketSize_Outgoing:
		return CachedPacketSizeOutgoing;
	}

	return 0.0f;
}

//////////////////////////////////////////////////////////////////////
// UOtterPerformanceStatSubsystem

void UOtterPerformanceStatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Tracker = MakeShared<FOtterPerformanceStatCache>(this);
	GEngine->AddPerformanceDataConsumer(Tracker);
}

void UOtterPerformanceStatSubsystem::Deinitialize()
{
	GEngine->RemovePerformanceDataConsumer(Tracker);
	Tracker.Reset();
}

double UOtterPerformanceStatSubsystem::GetCachedStat(EOtterDisplayablePerformanceStat Stat) const
{
	return Tracker->GetCachedStat(Stat);
}

