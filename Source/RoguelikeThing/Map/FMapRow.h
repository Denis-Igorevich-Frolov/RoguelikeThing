// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Map/FMapCellStructure.h"
#include "CoreMinimal.h"
#include "FMapRow.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable, BlueprintType)
struct ROGUELIKETHING_API FMapRow
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapCellStructure> row;
};
