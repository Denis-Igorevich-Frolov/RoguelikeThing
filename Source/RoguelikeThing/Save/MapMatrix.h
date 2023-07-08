// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Map/FMapRow.h"
#include "RoguelikeThing/Map/FMapCellStructure.h"
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MapMatrix.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public USaveGame
{
	GENERATED_BODY()

private:
	FMapCellStructure InvalidEmptyCell;
	TArray<FMapRow> map;
	
public:
	UMapMatrix();

	UFUNCTION(BlueprintCallable)
	const FMapCellStructure& GetCell (int32 row,  int32 col);

	UFUNCTION(BlueprintCallable)
	bool SetCell (FMapCellStructure Cell,  int32 row,  int32 col);

	UFUNCTION(BlueprintCallable)
	const TArray<FMapRow>& GetMap();

	UFUNCTION(BlueprintCallable)
	void SetMap(UPARAM(ref) TArray<FMapRow>& newMap);

	UFUNCTION(BlueprintCallable)
	int32 GetRowLength();

	UFUNCTION(BlueprintCallable)
	int32 GetColLength();

	UFUNCTION(BlueprintCallable)
	void addRow();

	UFUNCTION(BlueprintCallable)
	void addCol();

	UFUNCTION(BlueprintCallable)
	void addMultipleRows(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleCols(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void stretchRows(int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchCols(int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchMap(int32 rowsStretchTo,  int32 colsStretchTo);



	UFUNCTION(BlueprintCallable)
	void addRowToBeginning();

	UFUNCTION(BlueprintCallable)
	void addColToBeginning();

	UFUNCTION(BlueprintCallable)
	void addMultipleRowsToBeginning(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleColsToBeginning(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void stretchRowsAtBeginning(int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchColsAtBeginning(int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchMapAtBeginning(int32 rowsStretchTo,  int32 colsStretchTo);
};
