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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMapRow> map;

	UFUNCTION(BlueprintCallable)
	FMapCellStructure GetCell (UPARAM() int32 row, UPARAM() int32 col);

	UFUNCTION(BlueprintCallable)
	int32 GetRowLength();

	UFUNCTION(BlueprintCallable)
	int32 GetColLength();

	UFUNCTION(BlueprintCallable)
	void addRow();

	UFUNCTION(BlueprintCallable)
	void addCol();

	UFUNCTION(BlueprintCallable)
	void addMultipleRows(UPARAM() int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleCols(UPARAM() int32 quantity);

	UFUNCTION(BlueprintCallable)
	void stretchRows(UPARAM() int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchCols(UPARAM() int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchMap(UPARAM() int32 rowsStretchTo, UPARAM() int32 colsStretchTo);



	UFUNCTION(BlueprintCallable)
	void addRowToBeginning();

	UFUNCTION(BlueprintCallable)
	void addColToBeginning();

	UFUNCTION(BlueprintCallable)
	void addMultipleRowsToBeginning(UPARAM() int32 quantity);

	UFUNCTION(BlueprintCallable)
	void addMultipleColsToBeginning(UPARAM() int32 quantity);

	UFUNCTION(BlueprintCallable)
	void stretchRowsAtBeginning(UPARAM() int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchColsAtBeginning(UPARAM() int32 stretchTo);

	UFUNCTION(BlueprintCallable)
	void stretchMapAtBeginning(UPARAM() int32 rowsStretchTo, UPARAM() int32 colsStretchTo);
};
