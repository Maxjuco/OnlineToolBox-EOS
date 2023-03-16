// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineError.h"
#include "OnlineSubsystem.h"
#include "UObject/Object.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "OTStatsSubsystem.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOTUpdateStatsCompleteDelegate, bool, bWasSuccessful, FString, ErrorMsg);
UCLASS()
class ONLINETOOLBOX_API UOTStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UOTStatsSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Stats")
	void UpdateStats(FString StatName, int32 Amount);

	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOTUpdateStatsCompleteDelegate OnUpdateStatsComplete;

private:
	IOnlineStatsPtr StatsInterface;
	IOnlineIdentityPtr IdentityInterface;

	void HandleUpdateStatsComplete(const FOnlineError& ResultState) const;
};
