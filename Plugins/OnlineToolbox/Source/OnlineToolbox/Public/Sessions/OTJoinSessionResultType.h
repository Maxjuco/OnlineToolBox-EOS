// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OTJoinSessionResultType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EOTJoinSessionResultType
{
 Success UMETA(DisplayName = "Success"),
 SessionIsFull UMETA(DisplayName = "Full"),
 SessionDoesNotExist UMETA(DisplayName = "DoesNotExist"),
 CouldNotRetrieveAddress UMETA(DisplayName = "CouldNotRetrieveAddress"),
 AlreadyInSession UMETA(DisplayName = "AlreadyInSession"),
 UnknownError UMETA(DisplayName = "UnknownError")
};
