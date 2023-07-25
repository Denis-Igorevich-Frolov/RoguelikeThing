// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CellTypeOfMapStructure.h"
#include "SQLiteDatabase.h"
#include "MapMatrix.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(MapDataBase, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()

private:
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");
	int32 TableLength = 51;

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

	void mapDataBaseClose(FString FunctionName);
	void destroyLoadStatement(FString FunctionName);

public:
	UMapMatrix();
	~UMapMatrix();

	UFUNCTION(BlueprintCallable)
	bool CreateMapChunkStructure(int32 chunkRow, int32 chunkCol);
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, int32 value);
	UFUNCTION(BlueprintCallable)
	ECellTypeOfMapStructure GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol);
};
