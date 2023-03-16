﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Sessions/OTSessionsSubsystem.h"
#include "Sessions/OTSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Sessions/OTSessionSearchResult.h"
#include "Sessions/OTJoinSessionResultType.h"

UOTSessionsSubsystem::UOTSessionsSubsystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UOTSessionsSubsystem::OnCreateSessionComplete))
{
}

void UOTSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const auto* Subsystem = IOnlineSubsystem::Get();
	checkf(Subsystem != nullptr, TEXT("Unable to get subsystem"));

	SessionInterface = Subsystem->GetSessionInterface();
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;
}
void UOTSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOTSessionsSubsystem::CreateSession(int32 NumPublicConnections, const FString& MatchType)
{
	if(!ensureMsgf(SessionInterface.IsValid(), TEXT("Unable to get the Session Interface"))) return;

	// Destroy existing sessions
	const auto* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);

	if (ExistingSession != nullptr)
	{
		DestroySession();
		return;
	}

	// Register the delgate for when the creation and store its handle for later removal
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Get local player because create session need the creator unique net id
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	// Create the session parameters. We could not use the MakeShareable and thus the new
	// but this way we can make SessionSettings a member of our class for future reuse.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());

	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bShouldAdvertise = true;
#if !UE_BUILD_SHIPPING
	//Enforce a specific Build ID in not shipping so we can
	//easily test session creation
	LastSessionSettings->BuildUniqueId = 1;
#endif
	
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const bool bSuccess = SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession, *LastSessionSettings);

	if (bSuccess) return;

	//We failed to create the session simply remove the delegate for completion
	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

	//TODO : deal error case

}

void UOTSessionsSubsystem::FindSessions(int32 MaxSearchResults, const FString& MatchType)
{
	if (!ensureMsgf(SessionInterface.IsValid(),TEXT("Unable to get the session Interface"))) return;

	//Register the delegate for when the start session complete and store its handle for later removal
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	LastSessionSearch->QuerySettings.Set(FName("MatchType"),MatchType, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	const bool success = SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),LastSessionSearch.ToSharedRef());

	if (!success)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		TArray<FOTSessionSearchResult> Results;
		ToolboxOnFindSessionComplete.Broadcast(Results, false);
	}
}

void UOTSessionsSubsystem::JoinSession(const FOTSessionSearchResult& SessionResult)
{
	if (!ensureMsgf(SessionInterface.IsValid(),TEXT("Unable to get the session Interface"))) return;
	
	//Register the delegate for when the start session complete and store its handle for later removal
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	const bool success = SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(),NAME_GameSession, SessionResult.Native);

	if (!success)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		ToolboxOnJoinSessionComplete.Broadcast(false, EOTJoinSessionResultType::UnknownError, "");
	}
	
}

void UOTSessionsSubsystem::DestroySession()
{
	if (!ensureMsgf(SessionInterface.IsValid(),TEXT("Unable to get the session Interface"))) return;

	//Register the delegate for when the destroy session complete and store its handle for later removal 
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	//Destroy existing session if it exists
	const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	else //No session found
	{
		//Remove the destroy session completion delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);

		ToolboxOnDestroySessionComplete.Broadcast(false);
	}
}

void UOTSessionsSubsystem::StartSession()
{
	if (!ensureMsgf(SessionInterface.IsValid(),TEXT("Unable to get the session Interface"))) return;

	//Register the delegate for when the start session complete and store its handle for later removal
	StartSessionCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
		ToolboxOnStartSessionComplete.Broadcast(false);
	}
}

void UOTSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		//Remove the create session completion delegate
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	//Fire the delegate
	ToolboxOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UOTSessionsSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		//Remove the find session completion delegate
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() <=0)
	{
		TArray<FOTSessionSearchResult> Results;
		ToolboxOnFindSessionComplete.Broadcast(Results, false);
		return;
	}

	TArray<FOTSessionSearchResult> Results;
	Results.Reserve(LastSessionSearch->SearchResults.Num());
	for (auto& result : LastSessionSearch->SearchResults)
	{
		FOTSessionSearchResult r {};
		r.Native = result;
		r.Session = result.Session;
		r.PingInMs = result.PingInMs;
		Results.Add(r);
	}

	ToolboxOnFindSessionComplete.Broadcast(Results, true);
}

void UOTSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	else
	{
		ToolboxOnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Type::Success,UnknownError,"");
		return;
	}

	EOTJoinSessionResultType ResultBP = EOTJoinSessionResultType::UnknownError;
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
		ResultBP = Success;
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		ResultBP = SessionIsFull;
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		ResultBP = SessionDoesNotExist;
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		ResultBP = CouldNotRetrieveAddress;
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		ResultBP = AlreadyInSession;
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		ResultBP = UnknownError;
		break;
	}

	FString Address;
	SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

	// Fire delegate
	ToolboxOnJoinSessionComplete.Broadcast(Result == EOnJoinSessionCompleteResult::Type::Success, ResultBP,Address );
}

void UOTSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		//Remove the destroy session completion delegate
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	ToolboxOnDestroySessionComplete.Broadcast(bWasSuccessful);

	//if (!bWasSuccessful || !bCreateSessionOnDestroy) return;
	//CreateSession(LastNumPublicConnections);
}

void UOTSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		//Remove the start session completion delegate
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegateHandle);
	}

	ToolboxOnStartSessionComplete.Broadcast(bWasSuccessful);
}
