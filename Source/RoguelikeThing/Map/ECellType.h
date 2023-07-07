// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECellType.generated.h"

/**
 * 
 */
UENUM(Blueprintable, BlueprintType)
enum class ECellType : uint8
{
    Emptiness UMETA(DisplayName = "Emptiness"),
    Corridor UMETA(DisplayName = "Corridor"),
    RoomCenter UMETA(DisplayName = "RoomCenter"),
    RoomEdge UMETA(DisplayName = "RoomEdge"),
};
