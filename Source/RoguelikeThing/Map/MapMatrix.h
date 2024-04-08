// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"
#include "SQLiteDatabase.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
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
	ChunkStructure	UMETA(DisplayName = "ChunkStructure"), //Структура чанка карты
};

//Структура габаритов карты
USTRUCT(BlueprintType)
struct FMapDimensions
{
	GENERATED_BODY()

	//Минимальная координата стобца
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinCol;
    //Максимальная координата столбца
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxCol;
	//Минимальная координата строки
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinRow;
	//Максимальная координата строки
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxRow;
	/* Число строк и столбцов в матрице фрагмента карты. Это число всегда должно быть
	 * кратно длине тайла, если это не так, то оно усечётся до ближайшего кратного.
	 * Фрагменты карты нужны для хранения большого количества ячеек отдельным кластером в БД */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TableLength;
	/* Число строк и столбцов тайлов, на которые будет разбита карта. Она разбивается на маленькие
	 * тайлы для оптимизации - отображаться будут только те тайлы, которые сейчас видны */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MapTileLength;

	/* Переменная отражающая работоспособность структуры. Если она равна false,
	 * то она непроинициализирована и сгенерерована через пустой конструктор */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool isValid;

	FMapDimensions(int32 MinRow, int32 MaxRow, int32 MinCol, int32 MaxCol,
		int32 TableLength, int32 MapTileLength, bool isValid = true);

	/* Пустой конструктор по умолчанию.
	 * Созданный с помощью него экземпляр не будет считаться валидным.
	 * Создавать такой объект следует, если функция должна вернуть
	 * экземпляр MapDimensions, но её работа была выполнена неправильно */
	FMapDimensions();
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()

private:
	//Виджет загрузки, который проигрывается во время создания карты
	ULoadingWidget* LoadingWidget;
	bool SuccessCreateBlankCard = false;

	//Путь до файла, в котором лежит база данных карты
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");
	/* Число строк и столбцов в матрице фрагмента карты. Это число всегда должно быть
	 * кратно длине тайла, если это не так, то оно усечётся до ближайшего кратного.
	 * Фрагменты карты нужны для хранения большого количества ячеек отдельным кластером в БД */
	int32 TableLength = 50;
	/* Число строк и столбцов тайлов, на которые будет разбита карта. Она разбивается на маленькие
	 * тайлы для оптимизации - отображаться будут только те тайлы, которые сейчас видны */
	int32 MapTileLength = 5;

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	//Подготовленное заявление для загрузки данных из БД
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

	//Менеджер высокого уровня для экземпляра запущенной игры
	UMyGameInstance* GameInstance;

	//Функция, возвращающая название типа фрагмента карты по перечислению MatrixType
	FString getStringMatrixType(MatrixType matrixType);
	
	//Функция, закрывающая подготовленное заявление для загрузки данных из БД
	void destroyLoadStatement(FString FunctionName);

	//Функция, конвертирующая глобальный индекс базы данных карты в локальный индекс отдельного фрагмента
	void convertingGlobalIndexIntoLocalOne(int32 globalCellRow, int32 globalCellCol, int32& chunkRow,
		int32& cellRow, int32& chunkCol, int32& cellCol);

public:
	UMapMatrix();
	~UMapMatrix();

	UFUNCTION(BlueprintCallable)
	int32 GetTableLength();

	UFUNCTION(BlueprintCallable)
	int32 GetMapTileLength();

	UFUNCTION(BlueprintCallable)
	FMapDimensions GetMapDimensions(bool autoClose = true);

	//Функция, устанавливающая ссылку на виджет загрузки, который будет проигрываться при операциях с картой
	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);
	
	/* Функция, проверяющая корректность применения к определённой ячейке стиля корридора исходя из её окружения.
	 * Данная функция призвана не допустить создание неоднозначных путей (развилок) в коридорах. Переменную
	 * PassageDepthNumber не стоит трогать, это глубина прохода рекурсивной функции. Она нужна для того, чтобы
	 * проверить не только не нарушает ли новый коридор правила расположения, но и не вызывает ли он такое же
	 * нарушение у соседних клеток */
	UFUNCTION(BlueprintCallable)
	bool CheckCorrectOfCorridorLocation(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, bool autoClose = true, int PassageDepthNumber = 1);

	/* Функция, проверяющая корректность применения к определённой ячейке стиля комнаты исходя из её окружения.
	 * И хоть от самой комнаты развилки коридоров разрешены, но размещение новой комнаты рядом с коридором
	 * может приводить к созданию неоднозначных путей */
	UFUNCTION(BlueprintCallable)
	bool CheckCorrectOfRoomLocation(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, bool autoClose = true);

	//Функция, просматривающая есть ли непустые клетки вокруг указаной
	UFUNCTION(BlueprintCallable)
	FNeighbourhoodOfCell CheckNeighbourhoodOfCell(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, bool autoClose = true);

	/* Функция, создающая новый фрагмент карты на отснове переданного типа и индекса фрагмента.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	bool CreateMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose = true);

	/* Функция, удаляющая фрагмент карты на отснове переданного типа и индекса фрагмента.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	bool DeleteMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose = true);
	
	/* Функция, записывающая значение в ячейку фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCell(MatrixType matrixType,  int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, FMapEditorBrushType value, bool autoClose = true);
	
	/* Функция, считывающая значение из ячейки фрагмента БД по её локальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	FMapEditorBrushType GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose = true);
	
    //Функция, закрывающая базу данных карты
	UFUNCTION(BlueprintCallable)
	void mapDataBaseClose(FString FunctionName);

	/* Функция, записывающая значение в ячейку фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	bool SetValueOfMapChunkCellByGlobalIndex(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, FMapEditorBrushType value, bool autoClose = true);

	/* Функция, считывающая значение из ячейки фрагмента БД по её глобальному индексу.
	 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
	UFUNCTION(BlueprintCallable)
	FMapEditorBrushType GetValueOfMapChunkStructureCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, bool autoClose = true);

	//Функция, устанавливающая имя файла с базой данных
	UFUNCTION(BlueprintCallable)
	void SetFileName(FString fileName, bool WithExtension = false);

	//Функция, устанавливающая путь до файла с базой данных
	UFUNCTION(BlueprintCallable)
	void SetFilePath(FString filePath);

	//Функция, запускающая в отдельном потоке создание в базе даннх матрицы из фрагментов карты указанного типа
	UFUNCTION(BlueprintCallable)
	void AsyncCreateBlankCard(int32 rowLen, int32 colLen, MatrixType matrixType);
};
