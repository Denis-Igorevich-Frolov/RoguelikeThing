// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LogPrinter.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(BluePrintMessage, Log, All);

UENUM(BlueprintType)
enum class FMessageType : uint8 {
    LOG     UMETA(DisplayName = "Log"),
    WARNING UMETA(DisplayName = "Warning"),
    ERROR   UMETA(DisplayName = "Error"),
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API ULogPrinter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
    UFUNCTION(BlueprintCallable)
    void PrintLog(FString Message, FMessageType Type = FMessageType::LOG);
};
