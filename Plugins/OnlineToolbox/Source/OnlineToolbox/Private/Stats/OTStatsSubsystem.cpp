// Fill out your copyright notice in the Description page of Project Settings.


#include "Stats/OTStatsSubsystem.h"

#include "Interfaces/OnlineIdentityInterface.h"

UOTStatsSubsystem::UOTStatsSubsystem()
{
}

void UOTStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const auto* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get subsystem"));

	StatsInterface = Subsystem->GetStatsInterface();
	checkf(StatsInterface != nullptr, TEXT("Unable to get the Stats interface"));

	IdentityInterface = Subsystem->GetIdentityInterface();
	checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));
	
}

void UOTStatsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	
}

void UOTStatsSubsystem::UpdateStats(FString StatName, int32 Amount)
{
	checkf(StatsInterface != nullptr, TEXT("Unable to get the Stats interface"));
	checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));

	TArray<FOnlineStatsUserUpdatedStats> Stats;
	
	FOnlineStatsUserUpdatedStats Stat = FOnlineStatsUserUpdatedStats(IdentityInterface->GetUniquePlayerId(0).ToSharedRef());

	// Unknow = On laisse eos prendre le type qui faut (selon les parametres dans eos)
	Stat.Stats.Add(StatName,FOnlineStatUpdate(Amount, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

	Stats.Add(Stat);

	StatsInterface->UpdateStats(IdentityInterface->GetUniquePlayerId(0).ToSharedRef(),Stats,FOnlineStatsUpdateStatsComplete::CreateUObject(this,&UOTStatsSubsystem::HandleUpdateStatsComplete));
}

void UOTStatsSubsystem::HandleUpdateStatsComplete(const FOnlineError& ResultState) const
{
	if (!ResultState.bSucceeded)
	{
		
		OnUpdateStatsComplete.Broadcast(false, ResultState.GetErrorMessage().ToString());
		return;
	}
	OnUpdateStatsComplete.Broadcast(true, ResultState.GetErrorMessage().ToString());
}
