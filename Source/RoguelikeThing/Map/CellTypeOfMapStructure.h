// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECellTypeOfMapStructure : uint8 {
    Emptiness   = 0     UMETA(DisplayName = "Emptiness"),
    Error       = 1     UMETA(DisplayName = "Error"),
    Corridor    = 2     UMETA(DisplayName = "Corridor"),
    RoomEdge    = 3     UMETA(DisplayName = "RoomEdge"),
    RoomCenter  = 4     UMETA(DisplayName = "RoomCenter"),
};
