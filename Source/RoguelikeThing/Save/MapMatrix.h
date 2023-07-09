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

	///////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	void addRow();

	UFUNCTION(BlueprintCallable)
	void addCol();

	UFUNCTION(BlueprintCallable)
	void addMultipleRows(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleCols(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void enlargeRows(int32 enlargeTo);

	UFUNCTION(BlueprintCallable)
	void enlargeCols(int32 enlargeTo);

	UFUNCTION(BlueprintCallable)
	void enlargeMap(int32 enlargeRowsTo,  int32 enlargeColsTo);

	///////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	void addRowToBeginning();

	UFUNCTION(BlueprintCallable)
	void addColToBeginning();

	UFUNCTION(BlueprintCallable)
	void addMultipleRowsToBeginning(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleColsToBeginning(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void enlargeRowsAtBeginning(int32 enlargeTo);

	UFUNCTION(BlueprintCallable)
	void enlargeColsAtBeginning(int32 enlargeTo);

	UFUNCTION(BlueprintCallable)
	void enlargeMapAtBeginning(int32 enlargeRowsTo,  int32 enlargeColsTo);

	///////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	void deleteLastRow();

	UFUNCTION(BlueprintCallable)
	void deleteLastCol();

	UFUNCTION(BlueprintCallable)
	void deleteMultipleLastRows(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void deleteMultipleLastCols(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void truncateRowsFromEnd(int32 truncateTo);

	UFUNCTION(BlueprintCallable)
	void truncateColsFromEnd(int32 truncateTo);

	UFUNCTION(BlueprintCallable)
	void truncateMapFromEnd(int32 truncateRowsTo,  int32 truncateColsTo);

	///////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable)
	void deleteFirstRow();

	UFUNCTION(BlueprintCallable)
	void deleteFirstCol();

	UFUNCTION(BlueprintCallable)
	void deleteMultipleFirstRows(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void deleteMultipleFirstCols(int32 quantity);

	UFUNCTION(BlueprintCallable)
	void truncateRowsFromBeginning(int32 truncateTo);

	UFUNCTION(BlueprintCallable)
	void truncateColsFromBeginning(int32 truncateTo);

	UFUNCTION(BlueprintCallable)
	void truncateMapFromBeginning(int32 truncateRowsTo,  int32 truncateColsTo);
};
