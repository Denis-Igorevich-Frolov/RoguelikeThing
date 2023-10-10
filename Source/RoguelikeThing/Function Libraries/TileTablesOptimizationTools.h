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
USTRUCT(BlueprintType)
struct FTileCoord
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 X;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Y;

	FTileCoord(int32 x = 0, int32 y = 0);

	FString ToString();

	bool operator == (const FTileCoord& Coord) const;
	bool operator != (const FTileCoord& Coord) const;

	FTileCoord operator + (const FTileCoord Bias) const;
	FTileCoord operator - (const FTileCoord Bias) const;
};

USTRUCT(BlueprintType)
struct FDimensionsDisplayedArea
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Max;

	FDimensionsDisplayedArea(FTileCoord min = FTileCoord(), FTileCoord max = FTileCoord());

	FString ToString();
	bool IsEmpty();

	bool operator == (const FDimensionsDisplayedArea& Dimensions) const;
	bool operator != (const FDimensionsDisplayedArea& Dimensions) const;
	FDimensionsDisplayedArea operator + (const FDimensionsDisplayedArea Bias) const;
	FDimensionsDisplayedArea operator - (const FDimensionsDisplayedArea Bias) const;
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	int TileLen;
	int TableRows;
	int TableCols;

	FVector2D OriginalTableSize;
	FVector2D OriginalTileSize;
	FVector2D widgetAreaSize;

	FDimensionsDisplayedArea OldDimensions = FDimensionsDisplayedArea();
	FDimensionsDisplayedArea CurrentDimensions = FDimensionsDisplayedArea();
	FDimensionsDisplayedArea OriginalDimensions = FDimensionsDisplayedArea();
	
public:
	UFUNCTION(BlueprintCallable)
    void InitTableTiles(UUniformGridPanel* TileGridPanel, FVector2D TileSize,
		FVector2D WidgetAreaSize, FMapDimensions MapDimensions);

	UFUNCTION(BlueprintCallable)
	void ChangingVisibilityOfTableTiles(UUniformGridPanel* TileGridPanel, FVector2D Bias, float ZoomMultiplier);
};
