// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineError.h"
#include "OnlineSubsystem.h"
#include "UObject/Object.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "OTAchievementsSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOtOnAchievementsUnlockedCompleteDelegate, FString, AchievementId);
UCLASS()
class ONLINETOOLBOX_API UOTAchievementsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UOTAchievementsSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category="Achievements")
	FOtOnAchievementsUnlockedCompleteDelegate OnAchievementsUnlockedComplete;
	
private:
	IOnlineAchievementsPtr AchievementsInterface;
	IOnlineIdentityPtr IdentityInterface;

	FDelegateHandle AchievementsUnlockedDelegateHandle;

	FOnAchievementUnlockedDelegate AchievementUnlockedDelegate;

	FDelegateHandle AchievementsWrittenDelegateHandle;

	FOnAchievementsWrittenDelegate AchievementWrittenDelegate;
	
	
	void HandleUpdateAchievementsComplete(const FUniqueNetId& UserId, const FString& AchievementId) const;
};
