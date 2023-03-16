// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "UObject/Object.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Sessions/OTSessionSearchResult.h"
#include "Sessions/OTJoinSessionResultType.h"

#include "OTSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FToolboxOnFindSessionComplete, const TArray<FOTSessionSearchResult>&, SessionResults, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FToolboxOnJoinSessionComplete, bool, bWasSuccessful, EOTJoinSessionResultType, Type, const FString&, Adress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolboxOnStartSessionComplete, bool, bWasSuccessful);
UCLASS()
class ONLINETOOLBOX_API UOTSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UOTSessionsSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category="Session")
	void CreateSession(int32 NumPublicConnections, const FString& MatchType);

	UFUNCTION(BlueprintCallable, Category="Session")
	void FindSessions(int32 MaxSearchResults, const FString& MatchType);

	UFUNCTION(BlueprintCallable, Category="Session")
	void JoinSession(const FOTSessionSearchResult& SessionResult);

	UFUNCTION(BlueprintCallable, Category="Session")
	void DestroySession();

	UFUNCTION(BlueprintCallable, Category="Session")
	void StartSession();

	UPROPERTY(BlueprintAssignable)
	FToolboxOnCreateSessionComplete ToolboxOnCreateSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnDestroySessionComplete ToolboxOnDestroySessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnStartSessionComplete ToolboxOnStartSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnFindSessionComplete ToolboxOnFindSessionComplete;

	UPROPERTY(BlueprintAssignable)
	FToolboxOnJoinSessionComplete ToolboxOnJoinSessionComplete;
protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
    	
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;

};
