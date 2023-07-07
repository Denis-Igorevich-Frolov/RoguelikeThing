// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Map/ECellType.h"
#include "CoreMinimal.h"
#include "FMapCellStructure.generated.h"

/**
 *
 */

USTRUCT(Blueprintable, BlueprintType)
struct ROGUELIKETHING_API FMapCellStructure
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isValid = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECellType type = ECellType::Emptiness;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isExplored = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool ThereIsRightWall = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool ThereIsLeftWall = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool ThereIsRightDoor = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool ThereIsLeftDoor = false;
};

