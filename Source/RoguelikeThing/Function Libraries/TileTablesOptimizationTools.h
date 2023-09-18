// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "TileTablesOptimizationTools.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	int TileLen;
	int TableRows;
	int TableCols;
	FVector2D MinimumTableSize;

	FVector2D minimumTileSize;
	FVector2D widgetAreaSize;
	
public:
	UFUNCTION(BlueprintCallable)
	void InitializingUncollapseOfTiles(UUniformGridPanel* TileGridPanel, FVector2D MinimumTileSize,
		FVector2D WidgetAreaSize, FMapDimensions MapDimensions);
};
