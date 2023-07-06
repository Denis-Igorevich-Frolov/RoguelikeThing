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
    ECellType type;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isExplored = 0;
};

