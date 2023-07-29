// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CellTypeOfMapStructure.h"
#include "SQLiteDatabase.h"
#include "MapMatrix.generated.h"

/****************************************************************
 * Данный класс является классом взаимодействия с базой данных
 * карты мира. Здесь производится создание фрагментов карты,
 * получение значений из их ячеек и запись значений в них.
 * 
 * Сама карта состоит из множества фрагментов, отображающих
 * разные характеристики данной области карты, например
 * общую струтуру, состояние объектов взаимодействия, состояние
 * запертых дверей и т.д.
 ****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapDataBase, Log, All);

//Данное перечисление включает в себя все возможные типы фрагментов карты
UENUM(BlueprintType)
enum class MatrixType : uint8 {
	ChunkStructure	UMETA(DisplayName = "ChunkStructure"),//Структура карты
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()

private:
	//Путь до файла, в котором лежит база данных карты
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");
	//Число строк и столбцов в матрице фрагмента карты
	int32 TableLength = 51;

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	//Подготовленное заявление для загрузки данных из БД
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

	//Функция, возвращающая название типа фрагмента карты по перечислению MatrixType
	FString getStringMatrixType(MatrixType matrixType);
	
	//Функция, закрывающая подготовленное заявление для загрузки данных из БД
	void destroyLoadStatement(FString FunctionName);

	//Функция, конвертирующая глобальный индекс базы данных карты в локальный индекс отдельного фрагмента
	void convertingGlobalIndexIntoLocalOne(int32 globalCellRow, int32 globalCellCol, int32& chunkRow, int32& cellRow, int32& chunkCol, int32& cellCol);

public:
	UMapMatrix();
	~UMapMatrix();

	/* Функция, создающая новый фрагмент карты на отснове переданного типа и индекса фрагмента.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
	UFUNCTION(BlueprintCallable)
	bool CreateMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose = true);
	
	/* Функция, записывающая значение в ячейку фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCell(MatrixType matrixType,  int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, int32 value, bool autoClose = true);
	
	/* Функция, считывающая значение из ячейки фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
	UFUNCTION(BlueprintCallable)
	ECellTypeOfMapStructure GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose = true);
	
    //Функция, закрывающая базу данных карты
	UFUNCTION(BlueprintCallable)
	void mapDataBaseClose(FString FunctionName);

	/* Функция, записывающая значение в ячейку фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCellByGlobalIndex(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, int32 value, bool autoClose = true);

	/* Функция, считывающая значение из ячейки фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
	UFUNCTION(BlueprintCallable)
	ECellTypeOfMapStructure GetValueOfMapChunkStructureCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, bool autoClose = true);
};
