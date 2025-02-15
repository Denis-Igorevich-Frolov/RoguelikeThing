// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "MapMatrix.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include <Kismet/GameplayStatics.h>
#include <RoguelikeThing/Function Libraries/MyFileManager.h>
#include "Misc/SecureHash.h"

DEFINE_LOG_CATEGORY(MapMatrix);

UMapMatrix::UMapMatrix() : UObject()
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(MapMatrix, Warning, TEXT("Warning in FillingMapWithCells class in constructor - GameInstance was not retrieved from the world"));

    //Длина таблицы всегда должна быть кратна длине тайлов из которых она составлена
    int32 remainder = TableLength % MapTileLength;
    if (remainder != 0) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in constructor - TableLength is %d and not a multiple of MapTileLength equal to %d, it will be truncated to %d"), TableLength, MapTileLength, TableLength - remainder);
        //Если длина таблицы не кратна длине тайла, то она усекается до кратного
        TableLength = TableLength - remainder;
    }
}

UMapMatrix::~UMapMatrix()
{
    delete LoadStatement;

    mapDataBase->Close();
    delete mapDataBase;

    if(TerrainOfTilesContainer && TerrainOfTilesContainer->IsValidLowLevel())
        TerrainOfTilesContainer->TerrainOfTilesRows.Empty();
}

//Функция полного удаления текущего объекта. Всегда вызывать не обязательно, использовать по необходимости
void UMapMatrix::Destroy()
{
    if (this && IsValidLowLevel())
    {
        ConditionalBeginDestroy();
        MarkPendingKill();
    }
}

const FCellCoord UMapMatrix::GetMinNoEmptyTileCoord()
{
    return MinNoEmptyTileCoord;
}

const FCellCoord UMapMatrix::GetMaxNoEmptyTileCoord()
{
    return MaxNoEmptyTileCoord;
}

int32 UMapMatrix::GetTableLength()
{
    return TableLength;
}

int32 UMapMatrix::GetMapTileLength()
{
    return MapTileLength;
}

//Функция, закрывающая подготовленное заявление для загрузки данных из БД
void UMapMatrix::destroyLoadStatement(FString FunctionName)
{
    if (!LoadStatement->Destroy()) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in %s function - LoadStatement was not destroyed"), *FunctionName);
    }
    else
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the %s function: The LoadStatement object has been destroyed"), *FunctionName);
}

//Функция, конвертирующая глобальный индекс базы данных карты в локальный индекс отдельного фрагмента
void UMapMatrix::convertingGlobalIndexIntoLocalOne(int32 globalCellRow, int32 globalCellCol, int32& chunkRow, int32& cellRow, int32& chunkCol, int32& cellCol)
{
    globalCellRow++;
    globalCellCol++;

    if (globalCellRow > 0) {
        /* Для получения индекса фрагмента карты следует разделить глобальный индекс ячейки на размер таблицы.
         * Но так как переход на следующей индекс начинается не по достижении размера таблицы, а только ещё
         * через одно значение, например, размер таблицы равен 50, так перенос произойдёт на значении 51, из
         * переданного глобального индекса вычетается 1, чтобы отсрочить перенос индекса на эту единицу */
        chunkRow = (globalCellRow - 1) / TableLength;
        //Локальный индекс ячейки равен остатку от деления глобального индекса ячейки на размер таблицы
        cellRow = globalCellRow % TableLength;
    }
    else {
        /* Если глобальный индекс 0 или меньше, то откладывать перенос, как с положительным индексом, нет смысла,
         * так как 0 и так уже будет считаться одним из индексов. Но так как отрицательный глобальный индекс ячейки
         * всегда означает отрицательный индекс таблицы, а -1 будет получено только при достижении значения, большего,
         * чем размер таблицы, следует сразу вычесть единицу из итогового значения. */
        chunkRow = globalCellRow / TableLength - 1;
        /* Локальный индекс ячейки при отрицательном глобальном индексе, в отлиции от положительного, идёт справа налево,
         * а не слева направо, и так, учитывая что полученный от деления остаток всегда отрицательный, то его нужно
         * прибавить к длинне фрагмента, тем самым получив разницу между длинной фрагмента и модулем остатка от деления */
        cellRow = globalCellRow % TableLength + TableLength;
    }
    /* Если значение индекса ячейки равно 0, то это означает, что оно кратно длинне таблицы. В этом
     * случае нам как раз нужно число равное длинне таблицы для выставление корректного индекса */
    if (cellRow == 0)
        cellRow = TableLength;

    if (globalCellCol > 0) {
        /* Для получения индекса фрагмента карты следует разделить глобальный индекс ячейки на размер таблицы.
         * Но так как переход на следующей индекс начинается не по достижении размера таблицы, а только ещё
         * через одно значение, например, размер таблицы равен 51, так перенос произойдёт на значении 52, из
         * переданного глобального индекса вычетается 1, чтобы отсрочить перенос индекса на эту единицу. */
        chunkCol = (globalCellCol - 1) / TableLength;
        //Локальный индекс ячейки равен остатку от деления глобального индекса ячейки на размер таблицы
        cellCol = globalCellCol % TableLength;
    }
    else {
        /* Если глобальный индекс 0 или меньше, то откладывать перенос, как с положительным индексом, нет смысла,
         * так как 0 и так уже будет считаться одним из индексов. Но так как отрицательный глобальный индекс ячейки
         * всегда означает отрицательный индекс таблицы, а -1 будет получено только при достижении значения, большего,
         * чем размер таблицы, следует сразу вычесть единицу из итогового значения. */
        chunkCol = globalCellCol / TableLength - 1;
        /* Локальный индекс ячейки при отрицательном глобальном индексе, в отлиции от положительного, идёт справа налево,
         * а не слева направо, и так, учитывая что полученный от деления остаток всегда отрицательный, то его нужно
         * прибавить к длинне фрагмента, тем самым получив разницу между длинной фрагмента и модулем остатка от деления */
        cellCol = globalCellCol % TableLength + TableLength;
    }
    /* Если значение индекса ячейки равно 0, то это означает, что оно кратно длинне таблицы. В этом
     * случае нам как раз нужно число равное длинне таблицы для выставление корректного индекса */
    if (cellCol == 0)
        cellCol = TableLength;
}

//Функция, конвертирующая локальный индекс отдельного фрагмента в глобальный индекс базы данных карты
void UMapMatrix::convertingLocalIndexIntoGlobalOne(int32 chunkRow, int32 cellRow, int32 chunkCol, int32 cellCol, int32& globalCellRow, int32& globalCellCol)
{
    globalCellRow = chunkRow * TableLength + cellRow - 1;
    globalCellCol = chunkCol * TableLength + cellCol - 1;
}

/* Функция, создающая новый фрагмент карты на основе переданного индекса фрагмента. Стоит быть
 * внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
bool UMapMatrix::CreateMapChunk(int32 chunkRow, int32 chunkCol, bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть. */
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        /* Сначала проверяется не существует ли уже такая таблица путём попытки выделения в ней последней строки.
         * Именно из-за этой проверки, где неудачная попытка выделения говорит всего лишь о том, что такой
         * таблицы действительно ещё нет, и её можно спокойно дальше создавать, в логах, при инициализации
         * новой таблицы, отобразится предупреждение о провале операции SELECT. Просто игнорируете это
         * предупреждение, его появление говорит о корректной работе кода. */
        bool TableIsExists = LoadStatement->Create(*mapDataBase, *(FString::Printf(TEXT("SELECT * FROM \"%d:%d\" WHERE RowNum IS %d;"), chunkRow, chunkCol, TableLength)));
        if(TableIsExists)
            destroyLoadStatement("CreateMapChunk");

        if (!TableIsExists) {
            //Инициализация транзакции
            if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: A transaction was started to create table \"%d:%d\""), chunkRow, chunkCol);

            //Здесь начинает формироваться запрос на создание таблицы. Первый столбец RowNum отвечает за порядковый номер строки
            FString QueryToCreateTable = FString::Printf(TEXT(
                "CREATE TABLE IF NOT EXISTS \"%d:%d\"("
                "RowNum INTEGER PRIMARY KEY AUTOINCREMENT,"), chunkRow, chunkCol);

            /* Далее к запросу добавляется такое количество столбцов, какое указанно в TableLength. При
             * этом число 1 запрещено к записи, так как оно всегда зарезервированно под ошибку загрузки. */
            for (int i = 1; i <= TableLength; i++) {
                QueryToCreateTable += FString::Printf(TEXT("\"Col %d\" TEXT"), i, i);
                if (i != TableLength)
                    QueryToCreateTable += FString(TEXT(","));
            }

            QueryToCreateTable += FString(TEXT("); "));

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Generated query to create \"%d:%d\" table with %d columns"), chunkRow, chunkCol, TableLength);

            if (!mapDataBase->Execute(*QueryToCreateTable)) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to create the mapDataBase table: %s"), *mapDataBase->GetLastError());
                
                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }
                
                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            if (GameInstance && GameInstance->LogType != ELogType::NONE) {
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Query to create table \"%d:%d\" completed"), chunkRow, chunkCol);
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The consciousness of %d rows in the table \"%d:%d\" is started"), TableLength, chunkRow, chunkCol);
            }

            //После успешного создания всех столбцов, создаётся такое количество строк, какое указанно в TableLength
            for (int i = 0; i < TableLength; i++) {
                if (!mapDataBase->Execute(*(FString::Printf(TEXT("INSERT INTO \"%d:%d\" DEFAULT VALUES;"), chunkRow, chunkCol)))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to insert a row number %d into \"%d:%d\" table: %s"), i + 1, chunkRow, chunkCol, *mapDataBase->GetLastError());

                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                    }

                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }
            }

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of %d rows in the \"%d:%d\" table has been completed"), TableLength, chunkRow, chunkCol);

            //Далее проверяется есть ли в файле таблица габаритов карты
            bool DimensionTableExist = LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM Dimensions;"), ESQLitePreparedStatementFlags::Persistent);
            if (!DimensionTableExist) {
            /* Если таблицы нет, то начинается её создание. Эта таблица хранит
             * в себе минимальные и максимальные индексы столбцов и строк */
                if (!mapDataBase->Execute(TEXT(
                    "CREATE TABLE IF NOT EXISTS Dimensions("
                    "MinCol INTEGER NOT NULL,"
                    "MaxCol INTEGER NOT NULL,"
                    "MinRow INTEGER NOT NULL,"
                    "MaxRow INTEGER NOT NULL);"))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to create the Dimensions table: %s"), *mapDataBase->GetLastError());

                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                    }

                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Query to create table Dimensions completed"));

                /* В данной таблице всегда есть только одна строка. И исходя из того,
                 * что таблица была создана только сейчас, следует то, что текущий 
                 * фрагмент карты является первым и единственным, следовательно все
                 * его индексы и являются одновременно минимальными и максимальными.
                 * Так что они сразу записываются в эту строку. */
                if (!mapDataBase->Execute(*(FString::Printf(TEXT("INSERT INTO Dimensions (MinCol, MaxCol, MinRow, MaxRow) VALUES(%d, %d, %d, %d);"), chunkRow, chunkRow, chunkCol, chunkCol)))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to insert a row into Dimensions table: %s"), *mapDataBase->GetLastError());

                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                    }

                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of row with data row = %d, col = %d in the Dimensions table has been completed"), chunkRow, chunkCol);
            }
            else {
                /* Если таблица уже существует, то начинаются проверки являются
                 * ли текущие индексы новыми краями матрицы фрагментов */
                if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
                    //Проверка является ли текущий индекс столбца новым наименьшим индексом всех столбцов
                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkCol, equal to %d, is the smallest of the previously created column indexes"), chunkCol);
                    int32 MinCol;
                    bool CurrentColIndexSmallest = false;
                    if (LoadStatement->GetColumnValueByName(TEXT("MinCol"), MinCol)) {
                        CurrentColIndexSmallest = chunkCol < MinCol;
                        //Если индекс действительно наименьший, то он записывается в таблицу габаритов
                        if (CurrentColIndexSmallest) {
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new column index of %d is below the previous lowest index of %d. Started writing a new value for MinCol cell in table Dimensions"), chunkCol, MinCol);
                            if (!mapDataBase->Execute(
                                *FString::Printf(TEXT("UPDATE Dimensions SET MinCol = %d;"), chunkCol))) {
                                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MinCol: %s"), chunkCol, *mapDataBase->GetLastError());

                                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                }

                                if (autoClose)
                                    mapDataBaseClose("CreateMapChunk");
                                return false;
                            }
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MinCol cell in the Dimensions table of the value %d is complete"), chunkCol);
                        }else
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkCol index of %d is not lower than the previous smallest index of %d"), chunkCol, MinCol);
                    }
                    else {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MinCol from table Dimensions: %s"), *mapDataBase->GetLastError());

                        destroyLoadStatement("CreateMapChunk");
                        if (autoClose)
                            mapDataBaseClose("CreateMapChunk");
                        return false;
                    }

                    /* Проверка является ли текущий индекс столбцов новым наибольшим индексом всех столцов.
                     * Эта проверка не начинается, если индекс столбца уже был определён как наименьший,
                     * ведь единственный случай, когда столбец может быть одновременно и наибольшим, и
                     * наименьшим - это случай создания первого и пока что единственного фрагмента карты.
                     * Но этот вариант обрабатывается и записывается сразу при создании таблицы габаритов. */
                    if (!CurrentColIndexSmallest) {
                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkCol, equal to %d, is the highest of the previously created column indexes"), chunkCol);
                        int32 MaxCol;
                        if (LoadStatement->GetColumnValueByName(TEXT("MaxCol"), MaxCol)) {
                            if (chunkCol > MaxCol) {
                                //Если индекс действительно наибольший, то он записывается в таблицу габаритов
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new column index of %d is greater the previous highest index of %d. Started writing a new value for MaxCol cell in table Dimensions"), chunkCol, MaxCol);
                                if (!mapDataBase->Execute(
                                    *FString::Printf(TEXT("UPDATE Dimensions SET MaxCol = %d;"), chunkCol))) {
                                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MaxCol: %s"), chunkCol, *mapDataBase->GetLastError());

                                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                    }

                                    if (autoClose)
                                        mapDataBaseClose("CreateMapChunk");
                                    return false;
                                }
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MaxCol cell in the Dimensions table of the value %d is complete"), chunkCol);
                            }else
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkCol index of %d is not greater than the previous highest index of %d"), chunkCol, MaxCol);
                        }
                        else {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MaxCol from table Dimensions: %s"), *mapDataBase->GetLastError());

                            destroyLoadStatement("CreateMapChunk");
                            if (autoClose)
                                mapDataBaseClose("CreateMapChunk");
                            return false;
                        }
                    }

                    //Проверка является ли текущий индекс строки новым наименьшим индексом всех строк
                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkRow, equal to %d, is the smallest of the previously created row indexes"), chunkRow);
                    int32 MinRow;
                    bool CurrentRowIndexSmallest = false;
                    if (LoadStatement->GetColumnValueByName(TEXT("MinRow"), MinRow)) {
                        CurrentRowIndexSmallest = chunkRow < MinRow;
                        //Если индекс действительно наименьший, то он записывается в таблицу габаритов
                        if (CurrentRowIndexSmallest) {
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new row index of %d is below the previous lowest index of %d. Started writing a new value for MinRow cell in table Dimensions"), chunkRow, MinRow);
                            if (!mapDataBase->Execute(
                                *FString::Printf(TEXT("UPDATE Dimensions SET MinRow = %d;"), chunkRow))) {
                                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MinRow: %s"), chunkRow, *mapDataBase->GetLastError());

                                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                }

                                if (autoClose)
                                    mapDataBaseClose("CreateMapChunk");
                                return false;
                            }
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MinRow cell in the Dimensions table of the value %d is complete"), chunkRow);
                        }else
                            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkRow index of %d is not lower than the previous smallest index of %d"), chunkRow, MinRow);
                    }
                    else {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MinRow from table Dimensions: %s"), *mapDataBase->GetLastError());

                        destroyLoadStatement("CreateMapChunk");
                        if (autoClose)
                            mapDataBaseClose("CreateMapChunk");
                        return false;
                    }

                    /* Проверка является ли текущий индекс строки новым наибольшим индексом всех строк.
                     * Эта проверка не начинается, если индекс строки уже был определён как наименьший,
                     * ведь единственный случай, когда строка может быть одновременно и наибольшей, и
                     * наименьшей - это случай создания первого и пока что единственного фрагмента карты.
                     * Но этот вариант обрабатывается и записывается сразу при создании таблицы габаритов. */
                    if (!CurrentRowIndexSmallest) {
                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkRow, equal to %d, is the highest of the previously created row indexes"), chunkRow);
                        int32 MaxRow;
                        if (LoadStatement->GetColumnValueByName(TEXT("MaxRow"), MaxRow)) {
                            if (chunkRow > MaxRow) {
                                //Если индекс действительно наибольший, то он записывается в таблицу габаритов
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new row index of %d is greater the previous highest index of %d. Started writing a new value for MaxRow cell in table Dimensions"), chunkRow, MaxRow);
                                if (!mapDataBase->Execute(
                                    *FString::Printf(TEXT("UPDATE Dimensions SET MaxRow = %d;"), chunkRow))) {
                                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MaxRow: %s"), chunkRow, *mapDataBase->GetLastError());

                                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                    }

                                    if (autoClose)
                                        mapDataBaseClose("CreateMapChunk");
                                    return false;
                                }
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MaxRow cell in the Dimensions table of the value %d is complete"), chunkRow);
                            }else
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkRow index of %d is not greater than the previous highest index of %d"), chunkRow, MaxRow);
                        }
                        else {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MaxRow from table Dimensions: %s"), *mapDataBase->GetLastError());

                            destroyLoadStatement("CreateMapChunk");
                            if (autoClose)
                                mapDataBaseClose("CreateMapChunk");
                            return false;
                        }
                    }
                }
                else {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to execute a LoadStatement on table Dimensions: %s"), *mapDataBase->GetLastError());

                    destroyLoadStatement("CreateMapChunk");
                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                destroyLoadStatement("CreateMapChunk");
            }

            //Закрепление транзакции
            if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }

                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            if (GameInstance && GameInstance->LogType != ELogType::NONE) {
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The transaction to create the table \"%d:%d\" has been committed"), chunkRow, chunkCol);
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of the \"%d:%d\" table in the %s file is fully complete"), chunkRow, chunkCol, *FilePath);
            }
        }
        else
            /* Если таблица уже существует, false не возвращается. Главное чтобы после
             * выполнения этой функции указанная таблица так или иначе существовала */
                 if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                     UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CreateMapChunk function: Table \"%d:%d\" already exists in file %s"), chunkRow, chunkCol, *FilePath);
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateMapChunk");

    return true;
}

void UMapMatrix::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the setLoadWidget function: The download widget has been set"));
}

ULoadingWidget* UMapMatrix::getLoadWidget()
{
    return LoadingWidget;
}

/* Функция изменяющая размер карты в чанках. Метод способен это делать как в большую,
 * так и в меньшую сторону, но не способен уменьшить карту меньше, чем 1 на 1 чанк */
void UMapMatrix::AsyncChangeMatrixSize(UMapEditor* MapEditor, int right, int left, int top, int bottom)
{
    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: A change in the dimensions of the map matrix with input values has begun: right %d, left %d, top %d, bottom %d"), right, left, top, bottom);

    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, right, left, top, bottom, this]() {
        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: A thread to resizing the map matrix has been opened"));

        FMapDimensions Dimensions = GetMapDimensions(false);

        int NumOfRows = Dimensions.MaxRow - Dimensions.MinRow;
        int NumOfCols = Dimensions.MaxCol - Dimensions.MinCol;

        int Right = right;
        int Left = left;
        int Top = top;
        int Bottom = bottom;

        /* Если изменение карты приведёт к тому, что в ней не останется ни одного тайла, то переданные
         * значения усекаются до той степени, при которой в таблице сохранялся хотя бы 1 тайл */
        if ((NumOfCols + right) < 0) {
            Right = -NumOfCols;
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: Shifting to the right by %d will reduce the columns by a value less than 1. The shifting is truncated to %d"), right, Right);
        }
        if ((NumOfCols + left) < 0) {
            Left = -NumOfCols;
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: Shifting to the left by %d will reduce the columns by a value less than 1. The shifting is truncated to %d"), left, Left);
        }
        if ((NumOfRows + top) < 0) {
            Top = -NumOfRows;
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: Shifting to the top by %d will reduce the rows by a value less than 1. The shifting is truncated to %d"), top, Top);
        }
        if ((NumOfRows + bottom) < 0) {
            Bottom = -NumOfRows;
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: Shifting to the bottom by %d will reduce the rows by a value less than 1. The shifting is truncated to %d"), bottom, Bottom);
        }

        //Если при изменении габаритов карты, индексы уходят в минус, то это регистрируется для последующего смещения
        int RowsLessThanZero = 0;
        if ((Dimensions.MinRow - Top) < 0) {
            RowsLessThanZero = -(Dimensions.MinRow - Top);
        }

        int ColsLessThanZero = 0;
        if ((Dimensions.MinCol - Left) < 0) {
            ColsLessThanZero = -(Dimensions.MinCol - Left);
        }

        //Если при изменении габаритов карты, индексы уходят в минус, то все координаты тайлов смещаются таким образом, чтобы наименьшим индексом всегда был 0:0 
        if (RowsLessThanZero != 0 || ColsLessThanZero != 0) {
            ShiftDBCoords(RowsLessThanZero, ColsLessThanZero, true, false);
            Dimensions = GetMapDimensions(false);
        }

        //То же самое, если минимальный индекс стал больше 0:0. Только здесь смещение идёт в левый верхний угол
        int RowsGreaterThanZero = 0;
        if ((Dimensions.MinRow - Top) > 0) {
            RowsGreaterThanZero = Dimensions.MinRow - Top;
        }

        int ColsGreaterThanZero = 0;
        if ((Dimensions.MinCol - Left) > 0) {
            ColsGreaterThanZero = Dimensions.MinCol - Left;
        }

        if (RowsGreaterThanZero != 0 || ColsGreaterThanZero != 0) {
            ShiftDBCoords(RowsGreaterThanZero, ColsGreaterThanZero, false, false);
            Dimensions = GetMapDimensions(false);
        }

        if (Right > 0) {
            //Если смещение вправо больше 0, то добавляется столько правых столбцов, сколько было передано в аргументе
            for (int i = 0; i < Right; i++) {
                if (!CreateNewRightCol(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }
        else if (Right < 0) {
            //Если же меньше 0, то удаляется столько правых столбцов, сколько было передано в аргументе
            for (int i = Right; i < 0; i++) {
                if (!RemoveRightCol(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }

        //То же самое со всеми другими сторонами
        if (Left > 0) {
            for (int i = 0; i < Left; i++) {
                if (!CreateNewLeftCol(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }
        else if (Left < 0) {
            for (int i = Left; i < 0; i++) {
                if (!RemoveLeftCol(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }

        if (Top > 0) {
            for (int i = 0; i < Top; i++) {
                if (!CreateNewTopRow(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }
        else if (Top < 0) {
            for (int i = Top; i < 0; i++) {
                if (!RemoveTopRow(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }

        if (Bottom > 0) {
            for (int i = 0; i < Bottom; i++) {
                if (!CreateNewBottomRow(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }
        else if (Bottom < 0) {
            for (int i = Bottom; i < 0; i++) {
                if (!RemoveBottomRow(Dimensions, false)) {
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(false);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: The download widget has been removed"));
                    }

                    mapDataBaseClose("AsyncChangeMatrixSize");

                    return;
                }
                Dimensions = GetMapDimensions(false);
            }
        }

        AsyncTask(ENamedThreads::GameThread, [MapEditor, Dimensions, this]() {
            MapEditor->FillMapEditorWithCells(Dimensions);

            mapDataBaseClose("AsyncChangeMatrixSize");

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncChangeMatrixSize function: A thread to resizing the map matrix has been closed"));

            LoadingWidget->LoadingComplete(true);
            LoadingWidget->RemoveFromParent();
            });
        });
}

/* Функция, проверяющая корректность применения к определённой ячейке стиля корридора исходя из её окружения.
 * Данная функция призвана не допустить создание неоднозначных путей (развилок) в коридорах. Переменную
 * PassageDepthNumber не стоит трогать, это глубина прохода рекурсивной функции. Она нужна для того, чтобы
 * проверить не только не нарушает ли новый коридор правила расположения, но и не вызывает ли он такое же
 * нарушение у соседних клеток */
bool UMapMatrix::CheckCorrectOfCorridorLocation(int32 globalCellRow, int32 globalCellCol, int PassageDepthNumber)
{
    /* Если глубина прохода привышает 2, то рекурсия заканчивается. Как правило эта функция
     * вызывается только 3 раза считая самый первый, и на последнем вызове она заканчивается
     * здесь ничего не сделав, но если не надо проверять влияния текущей клетки на соседние,
     * то в функцию следует передать сразу глубину прохода 2, что заставит её сработать
     * полностью только 1 раз */
    if (PassageDepthNumber > 2) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: The function pass depth exceeded 2, which is the depth limit. The recursion has ended."));
        return true;
    }

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: Calling a function for cell Row: %d Col: %d, the current pass depth of the function is %d"), globalCellRow, globalCellCol, PassageDepthNumber);

    //Счётчик структур (клеток стиль которых отличин от пустого) вокруг текущей клетки
    int CounterOfSurroundingStructures = 0;
    FMapDimensions Dimensions = GetMapDimensions(false);
    int TotalRows = (Dimensions.MaxRow - Dimensions.MinRow + 1) * Dimensions.TableLength;
    int TotalCols = (Dimensions.MaxCol - Dimensions.MinCol + 1) * Dimensions.TableLength;

    //Проверка влияния нового коридора на клетку сверху. Если полученный индекс меньше 1, то это значит, что клеток выше текущей нет
    if (globalCellRow - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow - 1, globalCellCol));

        //На всякий случай всё равно проверяется валидность индекса
        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfCorridorLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow - 1, globalCellCol);

            return false;
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            //Если в этой верхней клетке есть коридор или комната, то счётчик окружающих структур регистрирует это
            CounterOfSurroundingStructures++;

            /* И только если клетка сверху является коридором, то необходимо проверить как текущая новая клетка
             * повлияет на старую верхнюю. Для этого функция вызывается рекурсивно с инкрементируемым счётчиком
             * глубины прохода. Для чего так делается леко показать на примере:
             *
             * Допустим есть следующий коридор
             *             ═══
             * Добавляем в него ответвление снизу
             *             ═╦═
             * 
             * Если проверять только как вляет окружение на новую клетку, то всё нормально - один коридор
             * переходит в другой и всё. Но если проверить как влияет новый коридор на страые клетки, то видно,
             * что у центрального коридора путь ведёт не в 2 других коридора, а в 3, что является неоднозначным
             * путём. Для того, чтобы избегать подобных ситуаций, функция и вызывается рекурсивно проверяя
             * валидность пути у клеток соседствующих с новой также как и для самóй новой.
             * 
             * Данная проверка рекурсивно вызывается только для коридоров, а не для комнат так как у комнат как
             * раз развилки разрешены */
            if (CellType == FCellType::Corridor) {
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, this function will be called recursively to check the validity of this cell, including adding the current one"), globalCellRow - 1, globalCellCol);

                //Если после рекурсивного вызова функции возвращается ложь, то это означает, что текущая клетка создала неоднозначный путь для верхней клетки
                if (!CheckCorrectOfCorridorLocation(globalCellRow - 1, globalCellCol, PassageDepthNumber + 1))
                    return false;
            }
        }
    }

    //Ниже такая же проверка проводится с нижней, левой и правой клетками

    if (globalCellRow + 1 < TotalRows) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow + 1, globalCellCol));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfCorridorLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow + 1, globalCellCol);

            return false;
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            CounterOfSurroundingStructures++;

            if (CellType == FCellType::Corridor) {
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, this function will be called recursively to check the validity of this cell, including adding the current one"), globalCellRow + 1, globalCellCol);

                if (!CheckCorrectOfCorridorLocation(globalCellRow + 1, globalCellCol, PassageDepthNumber + 1))
                    return false;
            }
        }
    }

    if (globalCellCol - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol - 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfCorridorLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol - 1);

            return false;
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            CounterOfSurroundingStructures++;

            if (CellType == FCellType::Corridor) {
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, this function will be called recursively to check the validity of this cell, including adding the current one"), globalCellRow, globalCellCol - 1);

                if (!CheckCorrectOfCorridorLocation(globalCellRow, globalCellCol - 1, PassageDepthNumber + 1))
                    return false;
            }
        }
    }

    if (globalCellCol + 1 < TotalCols) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol + 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfCorridorLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol + 1);

            return false;
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            CounterOfSurroundingStructures++;

            if (CellType == FCellType::Corridor) {
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfCorridorLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, this function will be called recursively to check the validity of this cell, including adding the current one"), globalCellRow, globalCellCol + 1);

                if (!CheckCorrectOfCorridorLocation(globalCellRow, globalCellCol + 1, PassageDepthNumber + 1))
                    return false;
            }
        }
    }

    //Если коридор ведёт в более чем 2 структуры, то путь считается неоднозначным
    if (CounterOfSurroundingStructures > 2)
        return false;
    else
        return true;
}

/* Функция, проверяющая корректность применения к определённой ячейке стиля комнаты исходя из её окружения.
 * И хоть от самой комнаты развилки коридоров разрешены, но размещение новой комнаты рядом с коридором
 * может приводить к созданию неоднозначных путей */
bool UMapMatrix::CheckCorrectOfRoomLocation(int32 globalCellRow, int32 globalCellCol)
{
    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfRoomLocation function: Calling a function for cell Row: %d Col: %d"), globalCellRow, globalCellCol);

    FMapDimensions Dimensions = GetMapDimensions(false);
    int TotalRows = (Dimensions.MaxRow - Dimensions.MinRow + 1) * Dimensions.TableLength;
    int TotalCols = (Dimensions.MaxCol - Dimensions.MinCol + 1) * Dimensions.TableLength;

    /* Хотя от комнаты развилки более чем возможны, расположение комнаты рядом с коридором может приводить к
     * созданию неоднозначного пути. Как так может получиться леко показать на примере:
     *
     * Допустим есть следующий коридор
     *             ═══
     * Добавляем снизу от него комнату
     *             ═╦═
     *              ▄
     * 
     * Со стороны комнаты всё нормально - от неё отходит один коридор и всё. Но если проверить как влияет новая
     * комната на страые клетки, то видно, что у центрального коридора путь ведёт не в 2 других структуры, а в 3,
     * что является неоднозначным путём.
     * Для того, чтобы избегать подобных ситуаций у всех коридоров окружающих текущую комнату вызывается проверка валидности */

     //Проверка влияния новой комнаты на клетку сверху. Если полученный индекс меньше 1, то это значит, что клеток выше текущей нет
    if (globalCellRow - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow - 1, globalCellCol));

        //На всякий случай всё равно проверяется валидность индекса
        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfRoomLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow - 1, globalCellCol);

            return false;
        }

        // Одна комната может спокойно граничить с другой в любой конфигурации, но у коридоров при расположении комнат могут возникнуть неоднозначные пути
        if (CellType == FCellType::Corridor) {
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfRoomLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, the CheckCorrectOfCorridorLocation function will be called to check of this cell, including adding the current one"), globalCellRow - 1, globalCellCol);

            /* Вызвается проверка валидности расположения коридоров для соседствующей клетки. При этом глубина прохода сразу
             * ставится 2, чтобы проверить валидность только для указаной клетки, без рекурсии ещё и не её соседей.
             * Если после вызова функции возвращается ложь, то это означает, что текущая клетка создала неоднозначный путь
             * для верхней клетки */
            if (!CheckCorrectOfCorridorLocation(globalCellRow - 1, globalCellCol, 2))
                return false;
        }
    }

    //Ниже такая же проверка проводится с нижней, левой и правой клетками

    if (globalCellRow + 1 < TotalRows) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow + 1, globalCellCol));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfRoomLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow + 1, globalCellCol);

            return false;
        }

        if (CellType == FCellType::Corridor) {
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfRoomLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, the CheckCorrectOfCorridorLocation function will be called to check of this cell, including adding the current one"), globalCellRow + 1, globalCellCol);

            if (!CheckCorrectOfCorridorLocation(globalCellRow + 1, globalCellCol, 2))
                return false;
        }
    }

    if (globalCellCol - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol - 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfRoomLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol - 1);

            return false;
        }

        if (CellType == FCellType::Corridor) {
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfRoomLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, the CheckCorrectOfCorridorLocation function will be called to check of this cell, including adding the current one"), globalCellRow, globalCellCol - 1);

            if (!CheckCorrectOfCorridorLocation(globalCellRow, globalCellCol - 1, 2))
                return false;
        }
    }

    if (globalCellCol + 1 < TotalCols) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol + 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckCorrectOfRoomLocation function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol + 1);

            return false;
        }

        if (CellType == FCellType::Corridor) {
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckCorrectOfRoomLocation function: A corridor was detected in the neighboring cell at index Row: %d Col: %d, the CheckCorrectOfCorridorLocation function will be called to check of this cell, including adding the current one"), globalCellRow, globalCellCol + 1);

            if (!CheckCorrectOfCorridorLocation(globalCellRow, globalCellCol + 1, 2))
                return false;
        }
    }

    //Если после вызова всех проверок ни разу не была возвращена ложь, то неоднозначных путей не обнаружено
    return true;
}

//Функция, просматривающая есть ли непустые клетки вокруг указаной
FNeighbourhoodOfCell UMapMatrix::CheckNeighbourhoodOfCell(int32 globalCellRow, int32 globalCellCol)
{
    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the CheckNeighbourhoodOfCell function: Calling a function for cell Row: %d Col: %d"), globalCellRow, globalCellCol);

    FMapDimensions Dimensions = GetMapDimensions(false);
    int TotalRows = (Dimensions.MaxRow - Dimensions.MinRow + 1) * Dimensions.TableLength;
    int TotalCols = (Dimensions.MaxCol - Dimensions.MinCol + 1) * Dimensions.TableLength;

    FNeighbourhoodOfCell NeighbourhoodOfCell = FNeighbourhoodOfCell();

    //Просмотр стиля клетки сверху. Если полученный индекс меньше 1, то это значит, что клеток выше текущей нет
    if (globalCellRow - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow - 1, globalCellCol));

        //На всякий случай всё равно проверяется валидность индекса
        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckNeighbourhoodOfCell function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow - 1, globalCellCol);

            //Если индекс не валиден, то возвращается пустая непроинициализированая структура
            return FNeighbourhoodOfCell();
        }

        //Если у клетки сверху тип коридор или комната, то это означает, что сверху от переданой клетки есть структура
        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            NeighbourhoodOfCell.SomethingOnTop = true;
        }
    }

    //Ниже такая же проверка проводится с нижней, левой и правой клетками

    if (globalCellRow + 1 < TotalRows) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow + 1, globalCellCol));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckNeighbourhoodOfCell function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow + 1, globalCellCol);

            return FNeighbourhoodOfCell();
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            NeighbourhoodOfCell.SomethingOnBottom = true;
        }
    }

    if (globalCellCol - 1 >= 0) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol - 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckNeighbourhoodOfCell function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol - 1);

            return FNeighbourhoodOfCell();
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            NeighbourhoodOfCell.SomethingOnLeft = true;
        }
    }

    if (globalCellCol + 1 < TotalCols) {
        FCellType CellType = GetCellStyleFromTerrainOfTile(FCellCoord(globalCellRow, globalCellCol + 1));

        if (CellType == FCellType::Error) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CheckNeighbourhoodOfCell function - The cell at the global index Row: %d Col: %d is not valid or its index is invalid"), globalCellRow, globalCellCol + 1);

            return FNeighbourhoodOfCell();
        }

        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
            NeighbourhoodOfCell.SomethingOnRight = true;
        }
    }

    //В конце ставится переменная, отражающая, что структура окружения клетки была проинициализирована
    NeighbourhoodOfCell.IsInit = true;

    return NeighbourhoodOfCell;
}

/* Функция, удаляющая фрагмент карты на основе переданного индекса фрагмента. Стоит быть
 * внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
bool UMapMatrix::DeleteMapChunk(int32 chunkRow, int32 chunkCol, bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть. */
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the DeleteMapChunk function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the DeleteMapChunk function: A transaction was started to delete table \"%d:%d\""), chunkRow, chunkCol);

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("DROP TABLE \"%d:%d\";"), chunkRow, chunkCol)))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to delete the mapDataBase table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the DeleteMapChunk function: Query to delete table \"%d:%d\" completed"), chunkRow, chunkCol);

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        if (GameInstance && GameInstance->LogType != ELogType::NONE) {
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the DeleteMapChunk function: The transaction to delete the table \"%d:%d\" has been committed"), chunkRow, chunkCol);
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the DeleteMapChunk function: The deleting of the \"%d:%d\" table in the %s file is fully complete"), chunkRow, chunkCol, *FilePath);
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("DeleteMapChunk");

    return true;
}

/* Функция, записывающая значение в ячейку фрагмента БД по её локальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
bool UMapMatrix::SetValueOfMapChunkCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, FCellType value, bool autoClose)
{
    FString Value;
    switch (value)
    {
    case FCellType::Corridor:
        Value = "Corridor";
        break;
    case FCellType::Room:
        Value = "Room";
        break;
    default:
        break;
    }
    //Все переданные индексы ячейки должны находиться в диапазоне от 1 до TableLength включительно
    if (cellRow < 1) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellRow value (%d) is less than 1"), cellRow);
        return false;
    }
    if (cellRow > TableLength) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellRow value (%d) is greater than TableLength (%d)"), cellRow , TableLength);
        return false;
    }
    if (cellCol < 1) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellCol value (%d) is less than 1"), cellCol);
        return false;
    }
    if (cellCol > TableLength) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellCol value (%d) is greater than TableLength (%d)"), cellCol , TableLength);
        return false;
    }

    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть. */
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWrite)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: A transaction to write data to table \"%d:%d\" has begun"), chunkRow, chunkCol);

        //Формирование запроса на запись данных в ячейку
        FString QueryToSetCellValue;
        /* Если значение равно Emptiness, то чтобы не загружать базу данных, в неё передаётся NULL,
         * который будет работать эквивалентно "Structure" : "Emptiness", но весить меньше */
        if(value == FCellType::Emptiness)
            QueryToSetCellValue = FString::Printf(TEXT("UPDATE \"%d:%d\" SET \"Col %d\" = NULL WHERE RowNum = %d;"), chunkRow, chunkCol, cellCol, cellRow);
        else {
            //Перед записью в ячаейку проверяется не пуста ли она
            LoadStatement->Create(*mapDataBase, *(FString::Printf(TEXT("SELECT * FROM \"%d:%d\" WHERE RowNum IS %d AND \"Col %d\" IS NOT NULL;"), chunkRow, chunkCol, cellRow, cellCol)));
            if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row)
                //Если в ячейке уже есть данные, то json, помещённый в ячейку, модифицируется на новое значение Structure
                QueryToSetCellValue = FString::Printf(TEXT("UPDATE \"%d:%d\" SET \"Col %d\" = json_replace(\"Col %d\", '$.Structure', \"%s\") WHERE RowNum = %d;"), chunkRow, chunkCol, cellCol, cellCol, *Value, cellRow);
            else
                //Иначе создаётся новый json со всеми необходимыми параметрами клетки
                QueryToSetCellValue = FString::Printf(TEXT("UPDATE \"%d:%d\" SET \"Col %d\" = json('{\"Structure\" : \"%s\",\"Test\" : \"e\"}') WHERE RowNum = %d;"), chunkRow, chunkCol, cellCol, *Value, cellRow);
            destroyLoadStatement("SetValueOfMapChunkCell");
        }

        if (!mapDataBase->Execute(*QueryToSetCellValue)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to set a value %s in the \"%d:%d\" table to cell %d:%d: %s"), *Value, chunkRow, chunkCol, cellRow, cellCol, *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The request to write data to table \"%d:%d\" has been completed"), chunkRow, chunkCol);

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            
            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The transaction to write data to table \"%d:%d\" was rolled back"), chunkRow, chunkCol);

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The transaction to write data to table \"%d:%d\" was committed"), chunkRow, chunkCol);
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The value %s is fully set to cell %d:%d in the \"%d:%d\" table"), *Value, cellRow, cellCol, chunkRow, chunkCol);
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("SetValueOfMapChunkCell");

    int32 globalCellRow;
    int32 globalCellCol;
    convertingLocalIndexIntoGlobalOne(chunkRow, cellRow, chunkCol, cellCol, globalCellRow, globalCellCol);
    //Если в массиве пересоздания тайлов уже существует информация о текущей ячейке, то она удаляется
    if (TerrainOfTilesContainer->ReCreationContainer.Contains(FVector2D(globalCellRow, globalCellCol)))
        TerrainOfTilesContainer->ReCreationContainer.Remove(FVector2D(globalCellRow, globalCellCol));
    //Затем в массив пересоздания тайлов помещается актуальная информация о стиле ячейки
    TerrainOfTilesContainer->ReCreationContainer.Add(FVector2D(globalCellRow, globalCellCol), value);

    return true;
}

/* Функция, считывающая значение из ячейки фрагмента БД по её локальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
FCellType UMapMatrix::GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose)
{
    //Все переданные индексы ячейки должны находиться в диапазоне от 1 до TableLength включительно
    if (cellRow < 1) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellRow value (%d) is less than 1"), cellRow);
        return FCellType::Error;
    }
    if (cellRow > TableLength) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellRow value (%d) is greater than TableLength (%d)"), cellRow, TableLength);
        return FCellType::Error;
    }
    if (cellCol < 1) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellCol value (%d) is less than 1"), cellCol);
        return FCellType::Error;
    }
    if (cellCol > TableLength) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellCol value (%d) is greater than TableLength (%d)"), cellCol, TableLength);
        return FCellType::Error;
    }

    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть. */
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return FCellType::Error;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        //Инициализация LoadStatement строкой с указанным индексом
        if (!LoadStatement->Create(*mapDataBase, *FString::Printf(TEXT("SELECT json_extract(\"Col %d\", '$.Structure') as Structure FROM \"%d:%d\" WHERE RowNum IS %d;"), cellCol, chunkRow, chunkCol, cellRow),
            ESQLitePreparedStatementFlags::Persistent) || !LoadStatement->IsValid()) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to initialize LoadStatement with table \"%d:%d\": %s"), chunkRow, chunkCol, *mapDataBase->GetLastError());

            if (autoClose)
                mapDataBaseClose("GetValueOfMapChunkStructureCell");
            return FCellType::Error;
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: A transaction to read data from the table \"%d:%d\" has begun"), chunkRow, chunkCol);

        if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
            FString result;
            //Получение указателя на перечисление FCellType для выполнения проверок
            const UEnum* CellType = FindObject<UEnum>(ANY_PACKAGE, TEXT("FCellType"), true);
            //Получение значения из выше выбранной строки по порядковому номеру столбца
            if (LoadStatement->GetColumnValueByIndex(0, result)) {

                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: The value %s was obtained at index %d:%d from the \"%d:%d\" table"), *result, cellRow, cellCol, chunkRow, chunkCol);


                if(result == "" || result == CellType->GetNameStringByIndex((int32)FCellType::Emptiness)){
                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");
                    return FCellType::Emptiness;
                }
                else if (result == CellType->GetNameStringByIndex((int32)FCellType::Corridor)) {
                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");
                    return FCellType::Corridor;
                }
                else if (result == CellType->GetNameStringByIndex((int32)FCellType::Room)) {
                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");
                    return FCellType::Room;
                }
                else {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to cast the value %s to the FCellType data type obtained at index %d:%d from the \"%d:%d\" table - this number is not valid for casting to FCellType"), *result, cellRow, cellCol, chunkRow, chunkCol);

                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");
                    return FCellType::Error;
                }
            }
            else {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to load the value at index %d:%d from table \"%d:%d\": %s"), cellRow, cellCol, chunkRow, chunkCol, *mapDataBase->GetLastError());

                destroyLoadStatement("GetValueOfMapChunkStructureCell");
                if (autoClose)
                    mapDataBaseClose("GetValueOfMapChunkStructureCell");
                return FCellType::Error;
            }
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to execute a LoadStatement on table \"%d:%d\": %s"), chunkRow, chunkCol, *mapDataBase->GetLastError());

            destroyLoadStatement("GetValueOfMapChunkStructureCell");
            if (autoClose)
                mapDataBaseClose("GetValueOfMapChunkStructureCell");
            return FCellType::Error;
        }

        destroyLoadStatement("GetValueOfMapChunkStructureCell");
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function - mapDataBase is not valid"));
        return FCellType::Error;
    }
}

//Функция, закрывающая базу данных
void UMapMatrix::mapDataBaseClose(FString FunctionName)
{
    if (mapDataBase->IsValid()) {
        if (!mapDataBase->Close()) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the %s function when trying to close mapDataBase: %s"), *FunctionName, *mapDataBase->GetLastError());
            return;
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the %s function: mapDataBase has been closed"), *FunctionName);
    }
    else {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in %s function - An attempt was made to close mapDataBase, but this database was not open"), *FunctionName);
    }
}

/* Функция, записывающая значение в ячейку фрагмента БД по её глобальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
bool UMapMatrix::SetValueOfMapChunkCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, FCellType value, bool autoClose)
{
    int32 chunkRow;
    int32 cellRow;
    int32 chunkCol;
    int32 cellCol;

    convertingGlobalIndexIntoLocalOne(globalCellRow, globalCellCol, chunkRow, cellRow, chunkCol, cellCol);

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function: Global index %d:%d translated into tables \"%d:%d\" cell %d:%d"), globalCellRow, globalCellCol, chunkRow, chunkCol, cellRow, cellCol);

    /* Если автозакрытие отключено, то база данных открывается здесь с модификатором ReadWriteCreate,
     * который подойдёт и для создания таблиц, и для записи данных в ячейки */
    if (!autoClose && !mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function: mapDataBase has been opened"));
    }

    /* Всегда перед попыткой записать данные в ячейку идёт попытка создать необходимую таблицу.
     * Если таблица уже существует, просто ничего не произойдёт, и запись данных продолжится */
    if (CreateMapChunk(chunkRow, chunkCol, autoClose))
        return SetValueOfMapChunkCell(chunkRow, chunkCol, cellRow, cellCol, value, autoClose);
    else
        return false;
}

/* Функция, считывающая значение из ячейки фрагмента БД по её глобальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически */
FCellType UMapMatrix::GetValueOfMapChunkStructureCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, bool autoClose)
{
    int32 chunkRow;
    int32 cellRow;
    int32 chunkCol;
    int32 cellCol;

    convertingGlobalIndexIntoLocalOne(globalCellRow, globalCellCol, chunkRow, cellRow, chunkCol, cellCol);

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCellByGlobalIndex function: Global index %d:%d translated into tables \"%d:%d\" cell %d:%d"), globalCellRow, globalCellCol, chunkRow, chunkCol, cellRow, cellCol);

    return GetValueOfMapChunkStructureCell(chunkRow, chunkCol, cellRow, cellCol, autoClose);
}

//Функция, устанавливающая имя папки, в которой хранится оригинальный файл базы данных
void UMapMatrix::SetOriginalDirName(FString originalDirName)
{
    this->OriginalDirName = originalDirName;

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetOriginalDirName function: The name of the original database dir is set to %s"), *OriginalDirName);
}

//Функция, возвращающая имя папки, в которой хранится оригинальный файл базы данных
FString UMapMatrix::GetOriginalDirName()
{
    return OriginalDirName;
}

//Функция, возвращающая полный путь до папки, в которой хранится оригинальный файл базы данных
FString UMapMatrix::GetOriginalDirPath()
{
    return FPaths::ProjectSavedDir() + TEXT("SaveGames/") + OriginalDirName;
}

/* Функция, устанавливающая имя директории с текущим файлом базы данных, и
 * по этому имени одновременно вычисляется полный путь до файла базы данных */
void UMapMatrix::SetFileDir(FString fileDir)
{
    FilePath = FPaths::ProjectSavedDir() + TEXT("/SaveGames/") + fileDir + TEXT("/Map.db");
    OriginalDirName = fileDir;
    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetFileDir function: The directory of the database file is set to %s, the path to the file is %s"), *fileDir, *FilePath);
}

/* Функция устанавливающая полный путь до файла базы данных, и по
 * этому пути одновременно вычисляется имя директории базы данных */
void UMapMatrix::SetFilePath(FString filePath)
{
    FilePath = filePath;

    TArray<FString> PartsOfPath;
    //Строка пути к файлу разбивается на отрезки, разделённые знаком '/'
    filePath.ParseIntoArray(PartsOfPath, TEXT("/"), false);
    //Директория файла сохранения будет предпоследней в массиве отрезков
    //Вычитается же 2, а не 1 потому что порядковый номер приводится к индексу
    if (PartsOfPath.IsValidIndex(PartsOfPath.Num() - 2)) {
        OriginalDirName = PartsOfPath[PartsOfPath.Num() - 2];
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("MapMatrix class in the SetFilePath function: The directory name of the database file was not recognized from the path %s"), *FilePath);
    }

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the SetFilePath function: The path to the database file is set as %s"), *FilePath);
}

//Функция, возвращающая полный путь до файла текущей базы данных, с которой непосредственно производятся манипуляции (может указывать на временный файл)
FString UMapMatrix::GetFilePath()
{
    return FilePath;
}

//Функция, создающая один новый солбец справа таблицы
bool UMapMatrix::CreateNewRightCol(FMapDimensions Dimensions, bool autoClose)
{
    if (Dimensions.isValid) {
        for (int row = Dimensions.MinRow; row <= Dimensions.MaxRow; row++) {
            if (!CreateMapChunk(row, Dimensions.MaxCol + 1, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateNewRightCol function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("CreateNewRightCol");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateNewRightCol");

    return true;
}

//Функция, создающая один новый солбец слева таблицы
bool UMapMatrix::CreateNewLeftCol(FMapDimensions Dimensions, bool autoClose)
{
    if (Dimensions.isValid) {
        for (int row = Dimensions.MinRow; row <= Dimensions.MaxRow; row++) {
            if (!CreateMapChunk(row, Dimensions.MinCol - 1, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateNewLeftCol function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("CreateNewLeftCol");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateNewLeftCol");

    return true;
}

//Функция, создающая одну новую строку сверху таблицы
bool UMapMatrix::CreateNewTopRow(FMapDimensions Dimensions, bool autoClose)
{
    if (Dimensions.isValid) {
        for (int col = Dimensions.MinCol; col <= Dimensions.MaxCol; col++) {
            if (!CreateMapChunk(Dimensions.MinRow - 1, col, false)) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateNewTopRow function - Dimensions is not valid"));
                if (autoClose)
                    mapDataBaseClose("CreateNewTopRow");
                return false;
            }
        }
    }
    else {
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateNewTopRow");

    return true;
}

//Функция, создающая одну новую строку снизу таблицы
bool UMapMatrix::CreateNewBottomRow(FMapDimensions Dimensions, bool autoClose)
{
    if (Dimensions.isValid) {
        for (int col = Dimensions.MinCol; col <= Dimensions.MaxCol; col++) {
            if (!CreateMapChunk(Dimensions.MaxRow + 1, col, false)) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateNewBottomRow function - Dimensions is not valid"));
                if (autoClose)
                    mapDataBaseClose("CreateNewBottomRow");
                return false;
            }
        }
    }
    else {
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateNewBottomRow");

    return true;
}

//Функция, удаляющая один солбец справа таблицы
bool UMapMatrix::RemoveRightCol(FMapDimensions Dimensions, bool autoClose)
{
    //Если текущее количество столбцов уже равно 1, то ничего не происходит
    if ((Dimensions.MaxCol - Dimensions.MinCol - 1) < 0) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in RemoveRightCol function - The current number of columns is already 1 and cannot become less"));
        return true;
    }

    if (Dimensions.isValid) {
        for (int row = Dimensions.MinRow; row <= Dimensions.MaxRow; row++) {
            if (!DeleteMapChunk(row, Dimensions.MaxCol, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveRightCol");
        return false;
    }

    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the RemoveRightCol function: mapDataBase has been opened"));
    }

    //При увеличении количества чанков, новое значение габаритов инициализируется автоматически. Но при удалении это стоит сделать вручную
    if (mapDataBase->IsValid()) {
        FMapDimensions Dimensions = GetMapDimensions(false);

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("RemoveRightCol");
            return false;
        }

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MaxCol = %d;"), Dimensions.MaxCol - 1)))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to update MaxCol in Dimensions table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveRightCol");
            return false;
        }

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveRightCol");
            return false;
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveRightCol function - mapDataBase is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveRightCol");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("RemoveRightCol");

    return true;
}

//Функция, удаляющая один солбец слева таблицы
bool UMapMatrix::RemoveLeftCol(FMapDimensions Dimensions, bool autoClose)
{
    //Если текущее количество столбцов уже равно 1, то ничего не происходит
    if ((Dimensions.MaxCol - Dimensions.MinCol - 1) < 0) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in RemoveLeftCol function - The current number of columns is already 1 and cannot become less"));
        return true;
    }

    if (Dimensions.isValid) {
        for (int row = Dimensions.MinRow; row <= Dimensions.MaxRow; row++) {
            if (!DeleteMapChunk(row, Dimensions.MinCol, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveLeftCol");
        return false;
    }

    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the RemoveLeftCol function: mapDataBase has been opened"));
    }

    //При увеличении количества чанков, новое значение габаритов инициализируется автоматически. Но при удалении это стоит сделать вручную
    if (mapDataBase->IsValid()) {
        FMapDimensions Dimensions = GetMapDimensions(false);

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("RemoveLeftCol");
            return false;
        }

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MinCol = %d;"), Dimensions.MinCol + 1)))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to update MinCol in Dimensions table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveLeftCol");
            return false;
        }

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveLeftCol");
            return false;
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveLeftCol function - mapDataBase is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveLeftCol");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("RemoveLeftCol");

    return true;
}

//Функция, удаляющая одну строку сверху таблицы
bool UMapMatrix::RemoveTopRow(FMapDimensions Dimensions, bool autoClose)
{
    //Если текущее количество строк уже равно 1, то ничего не происходит
    if ((Dimensions.MaxRow - Dimensions.MinRow - 1) < 0) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in RemoveTopRow function - The current number of rows is already 1 and cannot become less"));
        return true;
    }

    if (Dimensions.isValid) {
        for (int col = Dimensions.MinCol; col <= Dimensions.MaxCol; col++) {
            if (!DeleteMapChunk(Dimensions.MinRow, col, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveTopRow");
        return false;
    }

    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the RemoveTopRow function: mapDataBase has been opened"));
    }

    //При увеличении количества чанков, новое значение габаритов инициализируется автоматически. Но при удалении это стоит сделать вручную
    if (mapDataBase->IsValid()) {
        FMapDimensions Dimensions = GetMapDimensions(false);

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("RemoveTopRow");
            return false;
        }

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MinRow = %d;"), Dimensions.MinRow + 1)))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to update MinRow in Dimensions table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveTopRow");
            return false;
        }

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveTopRow");
            return false;
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveTopRow function - mapDataBase is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveTopRow");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("RemoveTopRow");

    return true;
}

//Функция, удаляющая одну строку снизу таблицы
bool UMapMatrix::RemoveBottomRow(FMapDimensions Dimensions, bool autoClose)
{
    //Если текущее количество строк уже равно 1, то ничего не происходит
    if ((Dimensions.MaxRow - Dimensions.MinRow - 1) < 0) {
        UE_LOG(MapMatrix, Warning, TEXT("Warning in MapMatrix class in RemoveBottomRow function - The current number of rows is already 1 and cannot become less"));
        return true;
    }

    if (Dimensions.isValid) {
        for (int col = Dimensions.MinCol; col <= Dimensions.MaxCol; col++) {
            if (!DeleteMapChunk(Dimensions.MaxRow, col, false)) {
                return false;
            }
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function - Dimensions is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveBottomRow");
        return false;
    }

    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the RemoveBottomRow function: mapDataBase has been opened"));
    }

    //При увеличении количества чанков, новое значение габаритов инициализируется автоматически. Но при удалении это стоит сделать вручную
    if (mapDataBase->IsValid()) {
        FMapDimensions Dimensions = GetMapDimensions(false);

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("RemoveBottomRow");
            return false;
        }

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MaxRow = %d;"), Dimensions.MaxRow - 1)))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to update MaxRow in Dimensions table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveBottomRow");
            return false;
        }

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("RemoveBottomRow");
            return false;
        }
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the RemoveBottomRow function - mapDataBase is not valid"));
        if (autoClose)
            mapDataBaseClose("RemoveBottomRow");
        return false;
    }

    if (autoClose)
        mapDataBaseClose("RemoveBottomRow");

    return true;
}

//Функция, сдвигающая координаты всех чанков в указанном направлении
bool UMapMatrix::ShiftDBCoords(int RowShift, int ColShift, bool ToRightBottom, bool autoClose)
{
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the ShiftDBCoords function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        FMapDimensions Dimensions = GetMapDimensions(false);

        if (Dimensions.MinRow + RowShift < 0 || Dimensions.MinCol + ColShift < 0 || Dimensions.MaxRow + RowShift < 0 || Dimensions.MaxCol + ColShift < 0) {
            return false;
        }

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("ShiftDBCoords");
            return false;
        }

        //Если сдвиг производится в сторону правого нижнего угла
        if (ToRightBottom) {
            //Изменения производятся начиная из правого нижнего угла, чтобы переименованые таблицы не затирали уже существующие
            for (int row = Dimensions.MaxRow; row >= Dimensions.MinRow; row--) {
                for (int col = Dimensions.MaxCol; col >= Dimensions.MinCol; col--) {
                    if (!mapDataBase->Execute(*(FString::Printf(TEXT("ALTER TABLE \"%d:%d\" RENAME TO \"%d:%d\";"), row, col, row + RowShift, col + ColShift)))) {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to move the Row: %d Col: %d coordinate to the Row: %d Col: %d point in table: %s"), row, col, row + RowShift, col + ColShift, *mapDataBase->GetLastError());

                        if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                        }

                        if (autoClose)
                            mapDataBaseClose("ShiftDBCoords");
                        return false;
                    }
                }
            }

            //После сдвига координат чанков, нужно привести в соответствие с этим и таблицу габаритов карты
            if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MinCol = %d, MaxCol = %d, MinRow = %d, MaxRow = %d;"),
                Dimensions.MinCol + ColShift, Dimensions.MaxCol + ColShift, Dimensions.MinRow + RowShift, Dimensions.MaxRow + RowShift)))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to update the Dimensions value to MinCol: %d, MaxCol: %d, MinRow: %d, MaxRow: %d: %s"), Dimensions.MinCol + ColShift, Dimensions.MaxCol + ColShift, Dimensions.MinRow + RowShift, Dimensions.MaxRow + RowShift, *mapDataBase->GetLastError());

                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }

                if (autoClose)
                    mapDataBaseClose("ShiftDBCoords");
                return false;
            }
        }
        //Если сдвиг производится в сторону левого верхнего угла
        else {
            for (int row = Dimensions.MinRow; row <= Dimensions.MaxRow; row++) {
                for (int col = Dimensions.MinCol; col <= Dimensions.MaxCol; col++) {
                    if (!mapDataBase->Execute(*(FString::Printf(TEXT("ALTER TABLE \"%d:%d\" RENAME TO \"%d:%d\";"), row, col, row - RowShift, col - ColShift)))) {
                        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to move the Row: %d Col: %d coordinate to the Row: %d Col: %d point in table: %s"), row, col, row - RowShift, col - ColShift, *mapDataBase->GetLastError());

                        if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                        }

                        if (autoClose)
                            mapDataBaseClose("ShiftDBCoords");
                        return false;
                    }
                }
            }

            //После сдвига координат чанков, нужно привести в соответствие с этим и таблицу габаритов карты
            if (!mapDataBase->Execute(*(FString::Printf(TEXT("UPDATE Dimensions SET MinCol = %d, MaxCol = %d, MinRow = %d, MaxRow = %d;"),
                Dimensions.MinCol - ColShift, Dimensions.MaxCol - ColShift, Dimensions.MinRow - RowShift, Dimensions.MaxRow - RowShift)))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to update the Dimensions value to MinCol: %d, MaxCol: %d, MinRow: %d, MaxRow: %d: %s"), Dimensions.MinCol - ColShift, Dimensions.MaxCol - ColShift, Dimensions.MinRow - RowShift, Dimensions.MaxRow - RowShift, *mapDataBase->GetLastError());

                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }

                if (autoClose)
                    mapDataBaseClose("ShiftDBCoords");
                return false;
            }
        }

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ShiftDBCoords function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("ShiftDBCoords");
            return false;
        }
    }
    else {
        return false;
    }

    if (autoClose)
        mapDataBaseClose("ShiftDBCoords");

    return true;
}

//Функция, запускающая в отдельном потоке создание в базе даннх матрицы из фрагментов карты
void UMapMatrix::AsyncCreateBlankCard(int32 rowLen, int32 colLen) {
    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Started asynchronous creation of %d rows by %d columns of map fragments"), rowLen+1, colLen+1);
    SuccessCreateBlankCard = false;

    //Запуск асинхронного потока
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [rowLen, colLen, this]() {
        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: A thread to creating map fragments has been opened"));
        for (int row = 0; row <= rowLen; row++) {
            for (int col = 0; col <= colLen; col++) {
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: The creation of the fragment \"%d:%d\" has begun"), row, col);
                SuccessCreateBlankCard = CreateMapChunk(row, col, false);
                //Если хотябы одна таблица не создалась, то дальнейшее создание таблиц прекращается
                if (!SuccessCreateBlankCard){
                    UE_LOG(MapMatrix, Error, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Fragment \"%d:%d\" was not created"), row, col);
                    break;
                }else
                    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Fragment \"%d:%d\" has been created"), row, col);
            }
            if (!SuccessCreateBlankCard)
                break;
        }

        mapDataBaseClose("AsyncCreateBlankCard");

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: The creation of all map fragments has been completed"));

        //Следующие функции запускаются в основном потоке так как могут быть выполнены только в нём
        AsyncTask(ENamedThreads::GameThread, [this]() {
            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Launched a GameThread call from the map fragments creation thread to switch widgets"));
            
            //Если все таблицы не были успешно созданы, то файл с ними удаляется
            if (!SuccessCreateBlankCard && !FilePath.IsEmpty()) {
                if (FPaths::ValidatePath(FilePath) && FPaths::FileExists(FilePath))
                {
                    IFileManager& FileManager = IFileManager::Get();
                    FileManager.Delete(*FilePath);
                }
            }

            //Виджет загрузки удаляется с экрана, и посылается сигнал окончания создания таблицы
            if (this->LoadingWidget) {
                LoadingWidget->LoadingComplete(SuccessCreateBlankCard);
                this->LoadingWidget->RemoveFromParent();
            }

            if (GameInstance && GameInstance->LogType != ELogType::NONE) {
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Widget switching done"));
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: A thread to creating map fragments has been closed"));
            }
            });
        });
}

//Функция заполняющая переменную предзагрузки TerrainOfTile для всех тайлов в таблице
void UMapMatrix::FillTerrainOfTiles(UMapSaver* MapSaver)
{
    if (!MapSaver) {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - MapSaver is not valid"));
        return;
    }

    //Получается хеш текущего файла базы данных
    FMD5Hash FileHash = FMD5Hash::HashFile(*FilePath);
    FString MapDataBaseHash = LexToString(FileHash);

    /* Производится сравнение хеша, оставшегося в файле сохранения с момента
     * последнего сохранения базы данных, и текущего хеша базы данных */
    //Если хеши равны, то переинициализация переменных предзагрузки не требуется, данные просто восстанавливаются из сериализованных
    if (MapSaver->MapDataBaseHash == MapDataBaseHash) {
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the FillTerrainOfTiles function: The hash of the database file has not changed, the table is loaded from the map.sav file"));
        TerrainOfTilesContainer = MapSaver->LoadTerrainOfTilesContainer();
        MinNoEmptyTileCoord = MapSaver->MinNoEmptyTileCoord;
        MaxNoEmptyTileCoord = MapSaver->MaxNoEmptyTileCoord;
    }
    //Если же хеши разные, то база данных была отредактирована извне и требуется полная переинициализация переменных предзагрузки
    else {
        UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the FillTerrainOfTiles function: The hash of the database file has changed, and the table is being completely reinitialized from the Map.db database"));
        //Сначала очищается матрица переменных предзагрузки от всех старых значений
        if (TerrainOfTilesContainer) {
            TerrainOfTilesContainer->Clear();
        }
        else {
            TerrainOfTilesContainer = NewObject<UTerrainOfTilesContainer>();
        }

        MinNoEmptyTileCoord = FCellCoord(-1, -1);
        MaxNoEmptyTileCoord = FCellCoord(-1, -1);

        FMapDimensions MapDimensions = GetMapDimensions(false);
        if (MapDimensions.isValid) {
            for (int row = MapDimensions.MinRow * TableLength; row < (MapDimensions.MaxRow + 1) * TableLength; row++) {
                int CurrentTileRow = (int)(row / MapTileLength);

                //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
                int CellRow = row % MapTileLength;

                //Если целевой строки нет, она создаётся
                if (!TerrainOfTilesContainer->TerrainOfTilesRows.Contains(CurrentTileRow)) {
                    UPROPERTY(SaveGame)
                    UTerrainOfTilesRow* Row = NewObject<UTerrainOfTilesRow>();
                    TerrainOfTilesContainer->TerrainOfTilesRows.Add(CurrentTileRow, Row);
                }

                UPROPERTY(SaveGame)
                UTerrainOfTilesRow* TerrainOfTilesRow;
                if (TerrainOfTilesContainer->TerrainOfTilesRows.Contains(CurrentTileRow)) {
                    TerrainOfTilesRow = *TerrainOfTilesContainer->TerrainOfTilesRows.Find(CurrentTileRow);
                }
                else {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - Array TerrainOfTilesContainer->TerrainOfTilesRows not contains valeu with key %d"), CurrentTileRow);
                    break;
                }

                if (TerrainOfTilesRow) {
                    for (int col = MapDimensions.MinCol * TableLength; col < (MapDimensions.MaxCol + 1) * TableLength; col++) {
                        int CurrentTileCol = (int)(col / MapTileLength);

                        //Если целевой переменной предзагрузки нет, она создаётся
                        if (!TerrainOfTilesRow->TerrainOfTilesRow.Contains(CurrentTileCol)) {
                            UPROPERTY(SaveGame)
                            UTerrainOfTile* Terrain = NewObject<UTerrainOfTile>();
                            TerrainOfTilesRow->TerrainOfTilesRow.Add(CurrentTileCol, Terrain);
                        }

                        UPROPERTY(SaveGame)
                        UTerrainOfTile* Terrain;
                        if (TerrainOfTilesRow->TerrainOfTilesRow.Contains(CurrentTileCol)) {
                            Terrain = *TerrainOfTilesRow->TerrainOfTilesRow.Find(CurrentTileCol);
                        }
                        else {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - Array TerrainOfTilesRow->TerrainOfTilesRow not contains valeu with key %d"), CurrentTileCol);
                            break;
                        }

                        if (Terrain) {
                            //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
                            int CellCol = col % MapTileLength;

                            FCellType CellType = GetValueOfMapChunkStructureCellByGlobalIndex(row, col, false);

                            TerrainOfTilesContainer->ReCreationContainer.Add(FVector2D(row, col), CellType);
                            if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
                                Terrain->AddCellType(FCellCoord(CellRow, CellCol), CellType);
                                //Проверка, не является ли текущий тайл минимальным или максимальным непустым тайлом
                                /* Если минимальные или максимальные координаты не проинициализированы ни одним
                                 * другим тайлом, то координаты текущего тайла автоматически записываются */
                                if (MinNoEmptyTileCoord == FCellCoord(-1, -1)) {
                                    MinNoEmptyTileCoord = FCellCoord(CurrentTileRow, CurrentTileCol);
                                }
                                if (MaxNoEmptyTileCoord == FCellCoord(-1, -1)) {
                                    MaxNoEmptyTileCoord = FCellCoord(CurrentTileRow, CurrentTileCol);
                                }

                                if (CurrentTileRow < MinNoEmptyTileCoord.Row) {
                                    MinNoEmptyTileCoord = FCellCoord(CurrentTileRow, MinNoEmptyTileCoord.Col);
                                }
                                else if (CurrentTileRow > MaxNoEmptyTileCoord.Row) {
                                    MaxNoEmptyTileCoord = FCellCoord(CurrentTileRow, MaxNoEmptyTileCoord.Col);
                                }
                                if (CurrentTileCol < MinNoEmptyTileCoord.Col) {
                                    MinNoEmptyTileCoord = FCellCoord(MinNoEmptyTileCoord.Row, CurrentTileCol);
                                }
                                else if (CurrentTileCol > MaxNoEmptyTileCoord.Col) {
                                    MaxNoEmptyTileCoord = FCellCoord(MaxNoEmptyTileCoord.Row, CurrentTileCol);
                                }
                            }
                            else if (CellType == FCellType::Error) {
                                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - CellType is of type Error"));
                            }
                        }
                        else {
                            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - Terrain is not valid"));
                        }
                    }
                }
                else {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - TerrainOfTilesRow is not valid"));
                }
            }

            SaveMap(MapSaver);
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - MapDimensions is not valid"));
        }
    }
}

//Проверка наличия переменной предзагрузки по переданной координате
bool UMapMatrix::ContainsTerrainOfTile(FCellCoord Coord)
{
    if (TerrainOfTilesContainer->TerrainOfTilesRows.Contains(Coord.Row)) {
        UTerrainOfTilesRow* TerrainOfTilesRow = *TerrainOfTilesContainer->TerrainOfTilesRows.Find(Coord.Row);
        if (TerrainOfTilesRow) {
            if (TerrainOfTilesRow->TerrainOfTilesRow.Contains(Coord.Col)) {
                return true;
            }
            else
                return false;
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ContainsTerrainOfTile function - TerrainOfTilesRow is not valid"));
            return false;
        }
    }
    else
        return false;
}

//Проверка наличия не пустой ячейки в матрице переменных предзагрузки по глобальному индексу ячейки
bool UMapMatrix::ContainsCellInTerrainOfTile(FCellCoord GlobalCoordOfCell)
{
    FCellCoord TileCoord = FCellCoord((int)(GlobalCoordOfCell.Row / MapTileLength), (int)(GlobalCoordOfCell.Col / MapTileLength));
    FCellCoord LocalCelCoord = FCellCoord(GlobalCoordOfCell.Row % MapTileLength, GlobalCoordOfCell.Col % MapTileLength);

    if (ContainsTerrainOfTile(TileCoord)) {
        UTerrainOfTile* Terrain = GetTerrainOfTile(TileCoord);
        if (Terrain) {
            return Terrain->Contains(LocalCelCoord);
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the ContainsTerrainOfTile function - Terrain is not valid"));
            return false;
        }
    }
    else {
        return false;
    }
}

//Получение стиля ячейки из матрицы переменных предзагрузки по глобальному индексу ячейки
FCellType UMapMatrix::GetCellStyleFromTerrainOfTile(FCellCoord GlobalCoordOfCell)
{
    if (ContainsCellInTerrainOfTile(GlobalCoordOfCell)) {
        FCellCoord TileCoord = FCellCoord((int)(GlobalCoordOfCell.Row / MapTileLength), (int)(GlobalCoordOfCell.Col / MapTileLength));
        FCellCoord LocalCelCoord = FCellCoord(GlobalCoordOfCell.Row % MapTileLength, GlobalCoordOfCell.Col % MapTileLength);

        UTerrainOfTile* Terrain = GetTerrainOfTile(TileCoord);
        return Terrain->GetCellType(LocalCelCoord);
    }
    else {
        //Если ячейки нет в матрице переменных предзагрузки, значит она пустая
        return FCellType::Emptiness;
    }
}

UTerrainOfTile* UMapMatrix::GetTerrainOfTile(FCellCoord Coord)
{
    if (ContainsTerrainOfTile(Coord)) {
        UTerrainOfTilesRow* TerrainOfTilesRow;
        if (TerrainOfTilesContainer->TerrainOfTilesRows.Contains(Coord.Row)) {
            TerrainOfTilesRow = *TerrainOfTilesContainer->TerrainOfTilesRows.Find(Coord.Row);
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetTerrainOfTile function - Array TerrainOfTilesContainer->TerrainOfTilesRows not contains valeu with key %d"), Coord.Row);
            return nullptr;
        }
        if (TerrainOfTilesRow) {
            UTerrainOfTile* TerrainOfTileRef;
            if (TerrainOfTilesRow->TerrainOfTilesRow.Contains(Coord.Col)) {
                TerrainOfTileRef = *TerrainOfTilesRow->TerrainOfTilesRow.Find(Coord.Col);
            }
            else {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetTerrainOfTile function - Array TerrainOfTilesRow->TerrainOfTilesRow not contains valeu with key %d"), Coord.Col);
                return nullptr;
            }
            if (TerrainOfTileRef) {
                UTerrainOfTile* Terrain = TerrainOfTileRef;
                if (Terrain) {
                    return Terrain;
                }
                else {
                    UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetTerrainOfTile function - Terrain is not valid"));
                    return nullptr;
                }
            }
            else {
                UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetTerrainOfTile function - TerrainOfTileRef is not valid"));
                return nullptr;
            }
        }
        else {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetTerrainOfTile function - TerrainOfTilesRow is not valid"));
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
}

/* Функция, считывающая координаты всего коридора. В CallingCellCoord следует передать
 * первую координату коридора, а в CurrentCellCoord - текущее местоположение пати. Если
 * на конце коридора будет встречена комната, то она добавится в конец массива */
TArray<FCellCoord> UMapMatrix::GetCorridorArray(FCellCoord CallingCellCoord, FCellCoord CurrentCellCoord)
{
    TArray<FCellCoord> CellsArray;
    //Сначала в массив добавляется ячейка, запустившая функцию
    CellsArray.Add(CallingCellCoord);

    /* Затем прощупываются координаты вокруг CallingCellCoord, и если обнаруженная
     * ячейка будет коридором и не будет являться предыдущей ячейкой поиска
     * CurrentCellCoord, то она добавится в массив и поиск продолжится */

    if ((FCellCoord(CallingCellCoord.Row + 1, CallingCellCoord.Col) != CurrentCellCoord) && (CallingCellCoord.Row + 1 <= (MaxNoEmptyTileCoord.Row + 1) * MapTileLength - 1)) {
        FCellType CellType = GetValueOfMapChunkStructureCellByGlobalIndex(CallingCellCoord.Row + 1, CallingCellCoord.Col);

        if (CellType == FCellType::Corridor) {
            /* Если обнаруженная ячейка является коридором, то данная функция рекурсивно запускается, передавая
             * в CallingCellCoord координату обнаруженной ячейки, а в CurrentCellCoord - текущую ячейку */
            CellsArray.Append(GetCorridorArray(FCellCoord(CallingCellCoord.Row + 1, CallingCellCoord.Col), CallingCellCoord));
        }
        else if (CellType == FCellType::Room) {
            /* Если во время поиска обнаруживается комната, это значит, что конец коридора найден.
             * Координата этой комнаты передаётся в массив, чтобы во время прохода по коридору у спавнера
             * коридоров была информация о том, куда перемещать пати после входа в конечную дверь */
            CellsArray.Add(FCellCoord(CallingCellCoord.Row + 1, CallingCellCoord.Col));
        }
    }
    //Аналогично для всех других сторон
    if ((FCellCoord(CallingCellCoord.Row - 1, CallingCellCoord.Col) != CurrentCellCoord) && (CallingCellCoord.Row - 1 >= MinNoEmptyTileCoord.Row * MapTileLength)) {
        FCellType CellType = GetValueOfMapChunkStructureCellByGlobalIndex(CallingCellCoord.Row - 1, CallingCellCoord.Col);

        if (CellType == FCellType::Corridor) {
            CellsArray.Append(GetCorridorArray(FCellCoord(CallingCellCoord.Row - 1, CallingCellCoord.Col), CallingCellCoord));
        }
        else if (CellType == FCellType::Room) {
            CellsArray.Add(FCellCoord(CallingCellCoord.Row - 1, CallingCellCoord.Col));
        }
    }
    if ((FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col + 1) != CurrentCellCoord) && (CallingCellCoord.Col + 1 <= (MaxNoEmptyTileCoord.Col + 1) * MapTileLength - 1)) {
        FCellType CellType = GetValueOfMapChunkStructureCellByGlobalIndex(CallingCellCoord.Row, CallingCellCoord.Col + 1);

        if (CellType == FCellType::Corridor) {
            CellsArray.Append(GetCorridorArray(FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col + 1), CallingCellCoord));
        }
        else if (CellType == FCellType::Room) {
            CellsArray.Add(FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col + 1));
        }
    }
    if ((FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col - 1) != CurrentCellCoord) && (CallingCellCoord.Col - 1 >= MinNoEmptyTileCoord.Col * MapTileLength)) {
        FCellType CellType = GetValueOfMapChunkStructureCellByGlobalIndex(CallingCellCoord.Row, CallingCellCoord.Col - 1);

        if (CellType == FCellType::Corridor) {
            CellsArray.Append(GetCorridorArray(FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col - 1), CallingCellCoord));
        }
        else if (CellType == FCellType::Room) {
            CellsArray.Add(FCellCoord(CallingCellCoord.Row, CallingCellCoord.Col - 1));
        }
    }

    //Рекурсия оканчивается, если обнаружена комната или все окружающие ячейки последней вызванной клетки коридора окажутся пустыми

    return CellsArray;
}

//Функция сохранение текущей карты в переданную переменную сохранения MapSaver
void UMapMatrix::SaveMap(UMapSaver* MapSaver)
{
    if (MapSaver) {
        //Для начала сохранения стоит освободить базу данных, закрыв её
        mapDataBaseClose("SaveMap");

        //Запоминается хеш файла базы данных для будущей проверки при загрузке
        FMD5Hash FileHash = FMD5Hash::HashFile(*FilePath);
        FString MapDataBaseHash = LexToString(FileHash);
        MapSaver->MapDataBaseHash = MapDataBaseHash;
        
        //Также запоминаются габариты карты и контейнер переменных предзагрузки карты
        MapSaver->MapDimensions = GetMapDimensions(true);
        MapSaver->SaveTerrainOfTilesContainer(TerrainOfTilesContainer);

        //Файл .sav помещается в ту же директорию, что и изначальная база данных
        UGameplayStatics::SaveGameToSlot(MapSaver, FString::Printf(TEXT("%s/map"), *OriginalDirName), 0);
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the SaveMap function - MapSaver is not valid"));
    }
}

FMapDimensions UMapMatrix::GetMapDimensions(bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть. */
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly)) {
            UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetMapDimensions function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return FMapDimensions();
        }
        else
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(MapMatrix, Log, TEXT("MapMatrix class in the GetMapDimensions function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        //Далее проверяется есть ли в файле таблица габаритов карты
        bool DimensionTableExist = LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM Dimensions;"), ESQLitePreparedStatementFlags::Persistent);
        if (DimensionTableExist) {
            if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
                int32 MinCol;
                if (!LoadStatement->GetColumnValueByName(TEXT("MinCol"), MinCol)) {
                    destroyLoadStatement("GetMapDimensions");
                    if (autoClose)
                        mapDataBaseClose("GetMapDimensions");
                    return FMapDimensions();
                }

                int32 MaxCol;
                if (!LoadStatement->GetColumnValueByName(TEXT("MaxCol"), MaxCol)) {
                    destroyLoadStatement("GetMapDimensions");
                    if (autoClose)
                        mapDataBaseClose("GetMapDimensions");
                    return FMapDimensions();
                }

                int32 MinRow;
                if (!LoadStatement->GetColumnValueByName(TEXT("MinRow"), MinRow)) {
                    destroyLoadStatement("GetMapDimensions");
                    if (autoClose)
                        mapDataBaseClose("GetMapDimensions");
                    return FMapDimensions();
                }

                int32 MaxRow;
                if (!LoadStatement->GetColumnValueByName(TEXT("MaxRow"), MaxRow)) {
                    destroyLoadStatement("GetMapDimensions");
                    if (autoClose)
                        mapDataBaseClose("GetMapDimensions");
                    return FMapDimensions();
                }

                destroyLoadStatement("GetMapDimensions");
                if (autoClose)
                    mapDataBaseClose("GetMapDimensions");
                return FMapDimensions(MinRow, MaxRow, MinCol, MaxCol, TableLength, MapTileLength);
            }
        }
        else
            return FMapDimensions();
    }
    else {
        UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetMapDimensions function - mapDataBase is not valid"));
    }

    return FMapDimensions();
}
