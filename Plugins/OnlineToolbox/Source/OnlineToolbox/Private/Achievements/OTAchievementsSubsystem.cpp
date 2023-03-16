// Fill out your copyright notice in the Description page of Project Settings.


#include "Achievements/OTAchievementsSubsystem.h"

#include "OnlineSubsystem.h"

UOTAchievementsSubsystem::UOTAchievementsSubsystem():
AchievementUnlockedDelegate(FOnAchievementUnlockedDelegate::CreateUObject(this,&UOTAchievementsSubsystem::HandleUpdateAchievementsComplete))
{
}

void UOTAchievementsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const auto* Subsystem = IOnlineSubsystem::Get();
    checkf(Subsystem != nullptr, TEXT("Unable to get subsystem"));

    AchievementsInterface = Subsystem->GetAchievementsInterface();
    checkf(this->AchievementsInterface != nullptr, TEXT("Unable to get the Achievements interface"));

    IdentityInterface = Subsystem->GetIdentityInterface();
    checkf(IdentityInterface != nullptr, TEXT("Unable to get the Identity interface"));
	
	AchievementsUnlockedDelegateHandle = AchievementsInterface->AddOnAchievementUnlockedDelegate_Handle(AchievementUnlockedDelegate);
}

void UOTAchievementsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	AchievementsInterface->ClearOnAchievementUnlockedDelegate_Handle(AchievementsUnlockedDelegateHandle);
}

void UOTAchievementsSubsystem::HandleUpdateAchievementsComplete(const FUniqueNetId& UserId,
	const FString& AchievementId) const
{
	UE_LOG(LogTemp, Warning, TEXT("Achievement %s unlocked for user %s"),*UserId.ToString(), *AchievementId)

	OnAchievementsUnlockedComplete.Broadcast(AchievementId);
}
