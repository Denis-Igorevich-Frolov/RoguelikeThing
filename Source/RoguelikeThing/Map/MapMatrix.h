// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "SQLiteDatabase.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
#include <RoguelikeThing/Widgets/MapEditor/CellCoord.h>
#include <RoguelikeThing/Widgets/MapEditor/MapEditor.h>
#include "RoguelikeThing/Map/MapDimensions.h"
#include "RoguelikeThing/Enumerations/CellType.h"
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

DECLARE_LOG_CATEGORY_EXTERN(MapMatrix, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(TerrainOfTile, Log, All);

//Класс предзагрузки структуры тайла. Хранит в себе все не нулевые ячейки тайла
UCLASS(BlueprintType)
class ROGUELIKETHING_API UTerrainOfTile : public UObject
{
	GENERATED_BODY()

private:
	TMap<int, TMap<int, FCellType>> TerrainOfTileRows;

public:
	UFUNCTION(BlueprintCallable)
	void AddCellType(FCellCoord Coord, FCellType CellType);
	UFUNCTION(BlueprintCallable)
	FCellType GetCellType(FCellCoord Coord);
	UFUNCTION(BlueprintCallable)
	bool Contains(FCellCoord Coord);
	//Получение массива всех не нулевых ячеек тайла
	UFUNCTION(BlueprintCallable)
	TArray<FCellCoord> GetFilledCoord();
	UFUNCTION(BlueprintCallable)
	bool RemoveCell(FCellCoord Coord);
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()

private:
	//Виджет загрузки, который проигрывается во время создания карты
	UPROPERTY()
	ULoadingWidget* LoadingWidget;
	bool SuccessCreateBlankCard = false;

	//Матрица переменных предзагрузки для всех тайлов текущей таблицы
	TMap<int, TMap<int, UTerrainOfTile*>> TerrainOfTilesRows;

	//Путь до файла, в котором лежит база данных карты
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");
	/* Число строк и столбцов в матрице фрагмента карты. Это число всегда должно быть
	 * кратно длине тайла, если это не так, то оно усечётся до ближайшего кратного.
	 * Фрагменты карты нужны для хранения большого количества ячеек отдельным кластером в БД */
	int32 TableLength = 30;
	/* Число строк и столбцов тайлов, на которые будет разбита карта. Она разбивается на маленькие
	 * тайлы для оптимизации - отображаться будут только те тайлы, которые сейчас видны */
	int32 MapTileLength = 5;

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	//Подготовленное заявление для загрузки данных из БД
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;
	
	//Функция, закрывающая подготовленное заявление для загрузки данных из БД
	void destroyLoadStatement(FString FunctionName);

	//Функция, конвертирующая глобальный индекс базы данных карты в локальный индекс отдельного фрагмента
	void convertingGlobalIndexIntoLocalOne(int32 globalCellRow, int32 globalCellCol, int32& chunkRow,
		int32& cellRow, int32& chunkCol, int32& cellCol);

	//Функция, сдвигающая координаты всех чанков в указанном направлении
	bool ShiftDBCoords(int RowShift, int ColShift, bool ToRightBottom, bool autoClose = true);

	//Минимальный и максимальный тайлы, в которых есть хотя бы 1 непустая ячейка
	FCellCoord MinNoEmptyTileCoord;
	FCellCoord MaxNoEmptyTileCoord;

public:
	UMapMatrix();
	~UMapMatrix();

	//Функция полного удаления текущего объекта. Всегда вызывать не обязательно, использовать по необходимости
	UFUNCTION(BlueprintCallable)
	void Destroy();

	UFUNCTION(BlueprintCallable)
	const FCellCoord GetMinNoEmptyTileCoord();
	UFUNCTION(BlueprintCallable)
	const FCellCoord GetMaxNoEmptyTileCoord();

	UFUNCTION(BlueprintCallable)
	int32 GetTableLength();

	UFUNCTION(BlueprintCallable)
	int32 GetMapTileLength();

	UFUNCTION(BlueprintCallable)
	FMapDimensions GetMapDimensions(bool autoClose = true);

	//Функция, устанавливающая ссылку на виджет загрузки, который будет проигрываться при операциях с картой
	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);
	UFUNCTION(BlueprintCallable)
	ULoadingWidget* getLoadWidget();
	
	/* Функция, проверяющая корректность применения к определённой ячейке стиля корридора исходя из её окружения.
	 * Данная функция призвана не допустить создание неоднозначных путей (развилок) в коридорах. Переменную
	 * PassageDepthNumber не стоит трогать, это глубина прохода рекурсивной функции. Она нужна для того, чтобы
	 * проверить не только не нарушает ли новый коридор правила расположения, но и не вызывает ли он такое же
	 * нарушение у соседних клеток */
	UFUNCTION(BlueprintCallable)
	bool CheckCorrectOfCorridorLocation(int32 globalCellRow, int32 globalCellCol, int PassageDepthNumber = 1);

	/* Функция, проверяющая корректность применения к определённой ячейке стиля комнаты исходя из её окружения.
	 * И хоть от самой комнаты развилки коридоров разрешены, но размещение новой комнаты рядом с коридором
	 * может приводить к созданию неоднозначных путей */
	UFUNCTION(BlueprintCallable)
	bool CheckCorrectOfRoomLocation(int32 globalCellRow, int32 globalCellCol);

	//Функция, просматривающая есть ли непустые клетки вокруг указаной
	UFUNCTION(BlueprintCallable)
	FNeighbourhoodOfCell CheckNeighbourhoodOfCell(int32 globalCellRow, int32 globalCellCol);

	/* Функция, создающая новый фрагмент карты на основе переданного индекса фрагмента. Стоит быть
	 * внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	bool CreateMapChunk(int32 chunkRow, int32 chunkCol, bool autoClose = true);

	/* Функция, удаляющая фрагмент карты на основе переданного индекса фрагмента. Стоит быть
	 * внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	bool DeleteMapChunk(int32 chunkRow, int32 chunkCol, bool autoClose = true);
	
	/* Функция, записывающая значение в ячейку фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, FCellType value, bool autoClose = true);
	
	/* Функция, считывающая значение из ячейки фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	FCellType GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose = true);
	
    //Функция, закрывающая базу данных карты
	UFUNCTION(BlueprintCallable)
	void mapDataBaseClose(FString FunctionName);

	/* Функция, записывающая значение в ячейку фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, FCellType value, bool autoClose = true);

	/* Функция, считывающая значение из ячейки фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	FCellType GetValueOfMapChunkStructureCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, bool autoClose = true);

	//Функция, устанавливающая имя файла с базой данных
	UFUNCTION(BlueprintCallable)
	void SetFileName(FString fileName, bool WithExtension = false);

	//Функция, устанавливающая путь до файла с базой данных
	UFUNCTION(BlueprintCallable)
	void SetFilePath(FString filePath);

	/* Функция изменяющая размер карты в чанках. Метод способен это делать как в большую,
	 * так и в меньшую сторону, но не способен уменьшить карту меньше, чем 1 на 1 чанк */
	UFUNCTION(BlueprintCallable)
	void AsyncChangeMatrixSize(UMapEditor* MapEditor, int right, int left, int top, int bottom);

	//Функция, создающая один новый солбец справа таблицы
	bool CreateNewRightCol(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, создающая один новый солбец слева таблицы
	bool CreateNewLeftCol(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, создающая одну новую строку сверху таблицы
	bool CreateNewTopRow(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, создающая одну новую строку снизу таблицы
	bool CreateNewBottomRow(FMapDimensions Dimensions, bool autoClose = true);

	//Функция, удаляющая один солбец справа таблицы
	bool RemoveRightCol(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, удаляющая один солбец слева таблицы
	bool RemoveLeftCol(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, удаляющая одну строку сверху таблицы
	bool RemoveTopRow(FMapDimensions Dimensions, bool autoClose = true);
	//Функция, удаляющая одну строку снизу таблицы
	bool RemoveBottomRow(FMapDimensions Dimensions, bool autoClose = true);

	//Функция, запускающая в отдельном потоке создание в базе даннх матрицы из фрагментов карты
	UFUNCTION(BlueprintCallable)
	void AsyncCreateBlankCard(int32 rowLen, int32 colLen);
	
	//Функция заполняющая переменную предзагрузки TerrainOfTile для всех тайлов в таблице
	UFUNCTION(BlueprintCallable)
    void FillTerrainOfTiles();
	
	//Проверка наличия переменной предзагрузки по переданной координате
	UFUNCTION(BlueprintCallable)
	bool ContainsTerrainOfTile(FCellCoord Coord);

	//Проверка наличия не пустой ячейки в матрице переменных предзагрузки по глобальному индексу ячейки
	UFUNCTION(BlueprintCallable)
	bool ContainsCellInTerrainOfTile(FCellCoord GlobalCoordOfCell);

	//Получение стиля ячейки из матрицы переменных предзагрузки по глобальному индексу ячейки
	UFUNCTION(BlueprintCallable)
	FCellType GetCellStyleFromTerrainOfTile(FCellCoord GlobalCoordOfCell);
	
	UFUNCTION(BlueprintCallable)
	UTerrainOfTile* GetTerrainOfTile(FCellCoord Coord);

	/* Функция, считывающая координаты всего коридора. В CallingCellCoord следует передать
	 * первую координату коридора, а в CurrentCellCoord - текущее местоположение пати. Если
	 * на конце коридора будет встречена комната, то она добавится в конец массива */
	UFUNCTION(BlueprintCallable)
	TArray<FCellCoord> GetCorridorArray(FCellCoord CallingCellCoord, FCellCoord CurrentCellCoord);
};
