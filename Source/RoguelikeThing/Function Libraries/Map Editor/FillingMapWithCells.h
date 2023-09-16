// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "FillingMapWithCells.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(FillingMapWithCells, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	ULoadingWidget* LoadingWidget;

	int ColSize;
	int RowSize;
	
public:
	UFUNCTION(BlueprintCallable)
	bool FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TileGridPanel,
		UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor);

	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);

	UFUNCTION(BlueprintCallable)
	int32 GetColSize();
	UFUNCTION(BlueprintCallable)
	int32 GetRowSize();
};
