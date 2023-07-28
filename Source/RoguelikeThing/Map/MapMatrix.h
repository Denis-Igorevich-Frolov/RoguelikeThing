// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CellTypeOfMapStructure.h"
#include "SQLiteDatabase.h"
#include "MapMatrix.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(MapDataBase, Log, All);

UENUM(BlueprintType)
enum class MatrixType : uint8 {
	ChunkStructure	UMETA(DisplayName = "ChunkStructure"),
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()

private:
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");
	int32 TableLength = 51;

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

	FString getStringMatrixType(MatrixType matrixType);
	void mapDataBaseClose(FString FunctionName);
	void destroyLoadStatement(FString FunctionName);

public:
	UMapMatrix();
	~UMapMatrix();

	UFUNCTION(BlueprintCallable)
	bool CreateMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose = true);
	
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCell(MatrixType matrixType,  int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, int32 value, bool autoClose = true);
	
	UFUNCTION(BlueprintCallable)
	ECellTypeOfMapStructure GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose = true);
	
	UFUNCTION(BlueprintCallable)
    void mapDataBaseManualClose();

	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCellByGlobalIndex(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, int32 value, bool autoClose = true);
};
