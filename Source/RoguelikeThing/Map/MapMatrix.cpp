// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMatrix.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"

DEFINE_LOG_CATEGORY(MapDataBase);

UMapMatrix::~UMapMatrix()
{
    delete LoadStatement;

    mapDataBase->Close();
    delete mapDataBase;
}

int32 UMapMatrix::GetTableLength()
{
    return TableLength;
}

//Функция, возвращающая название типа фрагмента карты по перечислению MatrixType
FString UMapMatrix::getStringMatrixType(MatrixType matrixType)
{
    switch (matrixType)
    {
    case MatrixType::ChunkStructure:
        return "Structure";
        break;
    default:
        return "";
        break;
    }
}

//Функция, закрывающая подготовленное заявление для загрузки данных из БД
void UMapMatrix::destroyLoadStatement(FString FunctionName)
{
    if (!LoadStatement->Destroy()) {
        UE_LOG(MapDataBase, Warning, TEXT("Warning in MapMatrix class in %s function - LoadStatement was not destroyed"), *FunctionName);
    }
    else
        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the %s function: The LoadStatement object has been destroyed"), *FunctionName);
}

//Функция, конвертирующая глобальный индекс базы данных карты в локальный индекс отдельного фрагмента
void UMapMatrix::convertingGlobalIndexIntoLocalOne(int32 globalCellRow, int32 globalCellCol, int32& chunkRow, int32& cellRow, int32& chunkCol, int32& cellCol)
{
    if (globalCellRow > 0) {
        /* Для получения индекса фрагмента карты следует разделить глобальный индекс ячейки на размер таблицы.
         * Но так как переход на следующей индекс начинается не по достижении размера таблицы, а только ещё
         * через одно значение, например, размер таблицы равен 51, так перенос произойдёт на значении 52, из
         * переданного глобального индекса вычетается 1, чтобы отсрочить перенос индекса на эту единицу*/
        chunkRow = (globalCellRow - 1) / TableLength;
        //Локальный индекс ячейки равен остатку от деления глобального индекса ячейки на размер таблицы
        cellRow = globalCellRow % TableLength;
    }
    else {
        /* Если глобальный индекс 0 или меньше, то откладывать перенос, как с положительным индексом, нет смысла,
         * так как 0 и так уже будет считаться одним из индексов. Но так как отрицательный глобальный индекс ячейки
         * всегда означает отрицательный индекс таблицы, а -1 будет получено только при достижении значения, большего,
         * чем размер таблицы, следует сразу вычесть единицу из итогового значения.*/
        chunkRow = globalCellRow / TableLength - 1;
        /* Локальный индекс ячейки при отрицательном глобальном индексе, в отлиции от положительного, идёт справа налево,
         * а не слева направо, и так, учитывая что полученный от деления остаток всегда отрицательный, то его нужно
         * прибавить к длинне фрагмента, тем самым получив разницу между длинной фрагмента и модулем остатка от деления*/
        cellRow = globalCellRow % TableLength + TableLength;
    }
    /* Если значение индекса ячейки равно 0, то это означает, что оно кратно длинне таблицы. В этом
     * случае нам как раз нужно число равное длинне таблицы для выставление корректного индекса*/
    if (cellRow == 0)
        cellRow = TableLength;

    if (globalCellCol > 0) {
        /* Для получения индекса фрагмента карты следует разделить глобальный индекс ячейки на размер таблицы.
         * Но так как переход на следующей индекс начинается не по достижении размера таблицы, а только ещё
         * через одно значение, например, размер таблицы равен 51, так перенос произойдёт на значении 52, из
         * переданного глобального индекса вычетается 1, чтобы отсрочить перенос индекса на эту единицу.*/
        chunkCol = (globalCellCol - 1) / TableLength;
        //Локальный индекс ячейки равен остатку от деления глобального индекса ячейки на размер таблицы
        cellCol = globalCellCol % TableLength;
    }
    else {
        /* Если глобальный индекс 0 или меньше, то откладывать перенос, как с положительным индексом, нет смысла,
         * так как 0 и так уже будет считаться одним из индексов. Но так как отрицательный глобальный индекс ячейки
         * всегда означает отрицательный индекс таблицы, а -1 будет получено только при достижении значения, большего,
         * чем размер таблицы, следует сразу вычесть единицу из итогового значения.*/
        chunkCol = globalCellCol / TableLength - 1;
        /* Локальный индекс ячейки при отрицательном глобальном индексе, в отлиции от положительного, идёт справа налево,
         * а не слева направо, и так, учитывая что полученный от деления остаток всегда отрицательный, то его нужно
         * прибавить к длинне фрагмента, тем самым получив разницу между длинной фрагмента и модулем остатка от деления*/
        cellCol = globalCellCol % TableLength + TableLength;
    }
    /* Если значение индекса ячейки равно 0, то это означает, что оно кратно длинне таблицы. В этом
     * случае нам как раз нужно число равное длинне таблицы для выставление корректного индекса*/
    if (cellCol == 0)
        cellCol = TableLength;
}

/* Функция, создающая новый фрагмент карты на отснове переданного типа и индекса фрагмента.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
bool UMapMatrix::CreateMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть.*/
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        FString SMatrixType = getStringMatrixType(matrixType);

        /* Если функия getStringMatrixType вернула значение "", то это значит, что в неё
         * попали некорретные данные и последующее выполнение функции стоит прекратить*/
        if (SMatrixType == "") {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function - incorrect map fragment type"));

            if (autoClose)
                mapDataBaseClose("CreateMapChunk");
            return false;
        }

        /* Сначала проверяется не существует ли уже такая таблица путём попытки выделения в ней последней строки.
         * Именно из-за этой проверки, где неудачная попытка выделения говорит всего лишь о том, что такой
         * таблицы действительно ещё нет, и её можно спокойно дальше создавать, в логах, при инициализации
         * новой таблицы, отобразится предупреждение о провале операции SELECT. Просто игнорируете это
         * предупреждение, его появление говорит о корректной работе кода.*/
        bool TableIsExists = LoadStatement->Create(*mapDataBase, *(FString::Printf(TEXT("SELECT * FROM \"%s %d:%d\" WHERE RowNum IS %d;"), *SMatrixType, chunkRow, chunkCol, TableLength)));
        if(TableIsExists)
            destroyLoadStatement("CreateMapChunk");

        if (!TableIsExists) {
            //Инициализация транзакции
            if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: A transaction was started to create table \"%s %d:%d\""), *SMatrixType, chunkRow, chunkCol);

            //Здесь начинает формироваться запрос на создание таблицы. Первый столбец RowNum отвечает за порядковый номер строки
            FString QueryToCreateTable = FString::Printf(TEXT(
                "CREATE TABLE IF NOT EXISTS \"%s %d:%d\"("
                "RowNum INTEGER PRIMARY KEY AUTOINCREMENT,"), *SMatrixType, chunkRow, chunkCol);

            /* Далее к запросу добавляется такое количество столбцов, какое указанно в TableLength. При
             * этом число 1 запрещено к записи, так как оно всегда зарезервированно под ошибку загрузки.*/
            for (int i = 1; i <= TableLength; i++) {
                QueryToCreateTable += FString::Printf(TEXT("\"Col %d\" INTEGER CHECK(\"Col %d\" !=1)"), i, i);
                if (i != TableLength)
                    QueryToCreateTable += FString(TEXT(","));
            }

            QueryToCreateTable += FString(TEXT("); "));

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Generated query to create \"%s %d:%d\" table with %d columns"), *SMatrixType, chunkRow, chunkCol, TableLength);

            if (!mapDataBase->Execute(*QueryToCreateTable)) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to create the mapDataBase table: %s"), *mapDataBase->GetLastError());
                
                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }
                
                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Query to create table \"%s %d:%d\" completed"), *SMatrixType, chunkRow, chunkCol);


            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started the consciousness of a request to add%d rows in the table \"%s %d:%d\" is started"), TableLength, *SMatrixType, chunkRow, chunkCol);
            //После успешного создания всех столбцов, создаётся такое количество строк, какое указанно в TableLength
            FString RequestToAddRows;
            for (int i = 0; i < TableLength; i++) {
                RequestToAddRows.Append(FString::Printf(TEXT("INSERT INTO \"%s %d:%d\" DEFAULT VALUES;"), *SMatrixType, chunkRow, chunkCol));
            }

            if (!mapDataBase->Execute(*RequestToAddRows)) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to insert a rows into \"%s %d:%d\" table: %s"), *SMatrixType, chunkRow, chunkCol, *mapDataBase->GetLastError());

                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }

                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of %d rows in the \"%s %d:%d\" table has been completed"), TableLength, *SMatrixType, chunkRow, chunkCol);

            //Далее проверяется есть ли в файле таблица габаритов карты
            bool DimensionTableExist = LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM Dimensions;"), ESQLitePreparedStatementFlags::Persistent);
            if (!DimensionTableExist) {
            /* Если таблицы нет, то начинается её создание. Эта таблица хранит
             * в себе минимальные и максимальные индексы столбцов и строк*/
                if (!mapDataBase->Execute(TEXT(
                    "CREATE TABLE IF NOT EXISTS Dimensions("
                    "MinCol INTEGER NOT NULL,"
                    "MaxCol INTEGER NOT NULL,"
                    "MinRow INTEGER NOT NULL,"
                    "MaxRow INTEGER NOT NULL);"))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to create the Dimensions table: %s"), *mapDataBase->GetLastError());

                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                    }

                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Query to create table Dimensions completed"));

                /* В данной таблице всегда есть только одна строка. И исходя из того,
                 * что таблица была создана только сейчас, следует то, что текущий 
                 * фрагмент карты является первым и единственным, следовательно все
                 * его индексы и являются одновременно минимальными и максимальными.
                 * Так что они сразу записываются в эту строку.*/
                if (!mapDataBase->Execute(*(FString::Printf(TEXT("INSERT INTO Dimensions (MinCol, MaxCol, MinRow, MaxRow) VALUES(%d, %d, %d, %d);"), chunkRow, chunkRow, chunkCol, chunkCol)))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to insert a row into Dimensions table: %s"), *mapDataBase->GetLastError());

                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                    }

                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of row with data row = %d, col = %d in the Dimensions table has been completed"), chunkRow, chunkCol);
            }
            else {
                /* Если таблица уже существует, то начинаются проверки являются
                 * ли текущие индексы новыми краями матрицы фрагментов*/
                if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
                    //Проверка является ли текущий индекс столбца новым наименьшим индексом всех столбцов
                    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkCol, equal to %d, is the smallest of the previously created column indexes"), chunkCol);
                    int32 MinCol;
                    bool CurrentColIndexSmallest = false;
                    if (LoadStatement->GetColumnValueByName(TEXT("MinCol"), MinCol)) {
                        CurrentColIndexSmallest = chunkCol < MinCol;
                        //Если индекс действительно наименьший, то он записывается в таблицу габаритов
                        if (CurrentColIndexSmallest) {
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new column index of %d is below the previous lowest index of %d. Started writing a new value for MinCol cell in table Dimensions"), chunkCol, MinCol);
                            if (!mapDataBase->Execute(
                                *FString::Printf(TEXT("UPDATE Dimensions SET MinCol = %d;"), chunkCol))) {
                                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MinCol: %s"), chunkCol, *mapDataBase->GetLastError());

                                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                }

                                if (autoClose)
                                    mapDataBaseClose("CreateMapChunk");
                                return false;
                            }
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MinCol cell in the Dimensions table of the value %d is complete"), chunkCol);
                        }else
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkCol index of %d is not lower than the previous smallest index of %d"), chunkCol, MinCol);
                    }
                    else {
                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MinCol from table Dimensions: %s"), *mapDataBase->GetLastError());

                        destroyLoadStatement("CreateMapChunk");
                        if (autoClose)
                            mapDataBaseClose("CreateMapChunk");
                        return false;
                    }

                    /* Проверка является ли текущий индекс столбцов новым наибольшим индексом всех столцов.
                     * Эта проверка не начинается, если индекс столбца уже был определён как наименьший,
                     * ведь единственный случай, когда столбец может быть одновременно и наибольшим, и
                     * наименьшим - это случай создания первого и пока что единственного фрагмента карты.
                     * Но этот вариант обрабатывается и записывается сразу при создании таблицы габаритов.*/
                    if (!CurrentColIndexSmallest) {
                        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkCol, equal to %d, is the highest of the previously created column indexes"), chunkCol);
                        int32 MaxCol;
                        if (LoadStatement->GetColumnValueByName(TEXT("MaxCol"), MaxCol)) {
                            if (chunkCol > MaxCol) {
                                //Если индекс действительно наибольший, то он записывается в таблицу габаритов
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new column index of %d is greater the previous highest index of %d. Started writing a new value for MaxCol cell in table Dimensions"), chunkCol, MaxCol);
                                if (!mapDataBase->Execute(
                                    *FString::Printf(TEXT("UPDATE Dimensions SET MaxCol = %d;"), chunkCol))) {
                                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MaxCol: %s"), chunkCol, *mapDataBase->GetLastError());

                                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                    }

                                    if (autoClose)
                                        mapDataBaseClose("CreateMapChunk");
                                    return false;
                                }
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MaxCol cell in the Dimensions table of the value %d is complete"), chunkCol);
                            }else
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkCol index of %d is not greater than the previous highest index of %d"), chunkCol, MaxCol);
                        }
                        else {
                            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MaxCol from table Dimensions: %s"), *mapDataBase->GetLastError());

                            destroyLoadStatement("CreateMapChunk");
                            if (autoClose)
                                mapDataBaseClose("CreateMapChunk");
                            return false;
                        }
                    }

                    //Проверка является ли текущий индекс строки новым наименьшим индексом всех строк
                    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkRow, equal to %d, is the smallest of the previously created row indexes"), chunkRow);
                    int32 MinRow;
                    bool CurrentRowIndexSmallest = false;
                    if (LoadStatement->GetColumnValueByName(TEXT("MinRow"), MinRow)) {
                        CurrentRowIndexSmallest = chunkRow < MinRow;
                        //Если индекс действительно наименьший, то он записывается в таблицу габаритов
                        if (CurrentRowIndexSmallest) {
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new row index of %d is below the previous lowest index of %d. Started writing a new value for MinRow cell in table Dimensions"), chunkRow, MinRow);
                            if (!mapDataBase->Execute(
                                *FString::Printf(TEXT("UPDATE Dimensions SET MinRow = %d;"), chunkRow))) {
                                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MinRow: %s"), chunkRow, *mapDataBase->GetLastError());

                                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                }

                                if (autoClose)
                                    mapDataBaseClose("CreateMapChunk");
                                return false;
                            }
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MinRow cell in the Dimensions table of the value %d is complete"), chunkRow);
                        }else
                            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkRow index of %d is not lower than the previous smallest index of %d"), chunkRow, MinRow);
                    }
                    else {
                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MinRow from table Dimensions: %s"), *mapDataBase->GetLastError());

                        destroyLoadStatement("CreateMapChunk");
                        if (autoClose)
                            mapDataBaseClose("CreateMapChunk");
                        return false;
                    }

                    /* Проверка является ли текущий индекс строки новым наибольшим индексом всех строк.
                     * Эта проверка не начинается, если индекс строки уже был определён как наименьший,
                     * ведь единственный случай, когда строка может быть одновременно и наибольшей, и
                     * наименьшей - это случай создания первого и пока что единственного фрагмента карты.
                     * Но этот вариант обрабатывается и записывается сразу при создании таблицы габаритов.*/
                    if (!CurrentRowIndexSmallest) {
                        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Started checking if chunkRow, equal to %d, is the highest of the previously created row indexes"), chunkRow);
                        int32 MaxRow;
                        if (LoadStatement->GetColumnValueByName(TEXT("MaxRow"), MaxRow)) {
                            if (chunkRow > MaxRow) {
                                //Если индекс действительно наибольший, то он записывается в таблицу габаритов
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The new row index of %d is greater the previous highest index of %d. Started writing a new value for MaxRow cell in table Dimensions"), chunkRow, MaxRow);
                                if (!mapDataBase->Execute(
                                    *FString::Printf(TEXT("UPDATE Dimensions SET MaxRow = %d;"), chunkRow))) {
                                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to set a value %d in the Dimensions table to cell MaxRow: %s"), chunkRow, *mapDataBase->GetLastError());

                                    if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                                        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                                    }

                                    if (autoClose)
                                        mapDataBaseClose("CreateMapChunk");
                                    return false;
                                }
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Writing to the MaxRow cell in the Dimensions table of the value %d is complete"), chunkRow);
                            }else
                                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The current chunkRow index of %d is not greater than the previous highest index of %d"), chunkRow, MaxRow);
                        }
                        else {
                            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to load the value at name MaxRow from table Dimensions: %s"), *mapDataBase->GetLastError());

                            destroyLoadStatement("CreateMapChunk");
                            if (autoClose)
                                mapDataBaseClose("CreateMapChunk");
                            return false;
                        }
                    }
                }
                else {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to execute a LoadStatement on table Dimensions: %s"), *mapDataBase->GetLastError());

                    destroyLoadStatement("CreateMapChunk");
                    if (autoClose)
                        mapDataBaseClose("CreateMapChunk");
                    return false;
                }

                destroyLoadStatement("CreateMapChunk");
            }

            //Закрепление транзакции
            if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

                if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
                }

                if (autoClose)
                    mapDataBaseClose("CreateMapChunk");
                return false;
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The transaction to create the table \"%s %d:%d\" has been committed"), *SMatrixType, chunkRow, chunkCol);            
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: The creation of the \"%s %d:%d\" table in the %s file is fully complete"), *SMatrixType, chunkRow, chunkCol, *FilePath);
        }
        else
            /* Если таблица уже существует, false не возвращается. Главное чтобы после
             * выполнения этой функции указанная таблица так или иначе существовала*/
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunk function: Table \"%s %d:%d\" already exists in file %s"), *SMatrixType, chunkRow, chunkCol, *FilePath);
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunk function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("CreateMapChunk");

    return true;
}

void UMapMatrix::setLoadWidget(ULoadingWidget* newDownloadWidget)
{
    this->DownloadWidget = newDownloadWidget;
}

/* Функция, удаляющая фрагмент карты на отснове переданного типа и индекса фрагмента.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
bool UMapMatrix::DeleteMapChunk(MatrixType matrixType, int32 chunkRow, int32 chunkCol, bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть.*/
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the DeleteMapChunk function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        FString SMatrixType = getStringMatrixType(matrixType);

        /* Если функия getStringMatrixType вернула значение "", то это значит, что в неё
         * попали некорретные данные и последующее выполнение функции стоит прекратить*/
        if (SMatrixType == "") {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function - incorrect map fragment type"));

            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the DeleteMapChunk function: A transaction was started to delete table \"%s %d:%d\""), *SMatrixType, chunkRow, chunkCol);

        if (!mapDataBase->Execute(*(FString::Printf(TEXT("DROP TABLE \"%s %d:%d\";"), *SMatrixType, chunkRow, chunkCol)))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to delete the mapDataBase table: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the DeleteMapChunk function: Query to delete table \"%s %d:%d\" completed"), *SMatrixType, chunkRow, chunkCol);

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("DeleteMapChunk");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the DeleteMapChunk function: The transaction to delete the table \"%s %d:%d\" has been committed"), *SMatrixType, chunkRow, chunkCol);
        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the DeleteMapChunk function: The deleting of the \"%s %d:%d\" table in the %s file is fully complete"), *SMatrixType, chunkRow, chunkCol, *FilePath);
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the DeleteMapChunk function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("DeleteMapChunk");

    return true;
}

/* Функция, записывающая значение в ячейку фрагмента БД по её локальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
bool UMapMatrix::SetValueOfMapChunkCell(MatrixType matrixType, int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, int32 value, bool autoClose)
{
    //Все переданные индексы ячейки должны находиться в диапазоне от 1 до TableLength включительно
    if (cellRow < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellRow value (%d) is less than 1"), cellRow);
        return false;
    }
    if (cellRow > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellRow value (%d) is greater than TableLength (%d)"), cellRow , TableLength);
        return false;
    }
    if (cellCol < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellCol value (%d) is less than 1"), cellCol);
        return false;
    }
    if (cellCol > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function - cellCol value (%d) is greater than TableLength (%d)"), cellCol , TableLength);
        return false;
    }

    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть.*/
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWrite)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        FString SMatrixType = getStringMatrixType(matrixType);

        /* Если функия getStringMatrixType вернула значение "", то это значит, что в неё
         * попали некорретные данные и последующее выполнение функции стоит прекратить*/
        if (SMatrixType == "") {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function - incorrect map fragment type"));

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        //Инициализация транзакции
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkCell function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: A transaction to write data to table \"%s %d:%d\" has begun"), *SMatrixType, chunkRow, chunkCol);

        //Формирование запроса на запись данных в ячейку
        FString QueryToSetCellValue = FString::Printf(TEXT(
            "UPDATE \"%s %d:%d\" SET \"Col %d\" = %d WHERE RowNum = %d;"), *SMatrixType, chunkRow, chunkCol, cellCol, value, cellRow);

        if (!mapDataBase->Execute(*QueryToSetCellValue)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to set a value %d in the \"%s %d:%d\" table to cell %d:%d: %s"), value, *SMatrixType, chunkRow, chunkCol, cellRow, cellCol, *mapDataBase->GetLastError());

            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The request to write data to table \"%s %d:%d\" has been completed"), *SMatrixType, chunkRow, chunkCol);

        //Закрепление транзакции
        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            
            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The transaction to write data to table \"%s %d:%d\" was rolled back"), *SMatrixType, chunkRow, chunkCol);

            if (autoClose)
                mapDataBaseClose("SetValueOfMapChunkCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The transaction to write data to table \"%s %d:%d\" was committed"), *SMatrixType, chunkRow, chunkCol);
        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCell function: The number %d is fully set to cell %d:%d in the \"%s %d:%d\" table"), value, cellRow, cellCol, *SMatrixType, chunkRow, chunkCol);
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCell function - mapDataBase is not valid"));
        return false;
    }

    if (autoClose)
        mapDataBaseClose("SetValueOfMapChunkCell");
    return true;
}

/* Функция, считывающая значение из ячейки фрагмента БД по её локальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
ECellTypeOfMapStructure UMapMatrix::GetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, bool autoClose)
{
    //Все переданные индексы ячейки должны находиться в диапазоне от 1 до TableLength включительно
    if (cellRow < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellRow value (%d) is less than 1"), cellRow);
        return ECellTypeOfMapStructure::Error;
    }
    if (cellRow > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellRow value (%d) is greater than TableLength (%d)"), cellRow, TableLength);
        return ECellTypeOfMapStructure::Error;
    }
    if (cellCol < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellCol value (%d) is less than 1"), cellCol);
        return ECellTypeOfMapStructure::Error;
    }
    if (cellCol > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the GetValueOfMapChunkStructureCell function - cellCol value (%d) is greater than TableLength (%d)"), cellCol, TableLength);
        return ECellTypeOfMapStructure::Error;
    }

    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть.*/
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return ECellTypeOfMapStructure::Error;
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: mapDataBase has been opened"));
    }

    //После открытия базы данных следует ещё раз проверить её валидность
    if (mapDataBase->IsValid()) {
        //Инициализация LoadStatement строкой с указанным индексом
        if (!LoadStatement->Create(*mapDataBase, *FString::Printf(TEXT("SELECT * FROM \"Structure %d:%d\" WHERE RowNum IS %d;"), chunkRow, chunkCol, cellRow),
            ESQLitePreparedStatementFlags::Persistent) || !LoadStatement->IsValid()) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to initialize LoadStatement with table \"Structure %d:%d\": %s"), chunkRow, chunkCol, *mapDataBase->GetLastError());

            if (autoClose)
                mapDataBaseClose("GetValueOfMapChunkStructureCell");
            return ECellTypeOfMapStructure::Error;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: A transaction to read data from the table \"Structure %d:%d\" has begun"), chunkRow, chunkCol);

        if (LoadStatement->IsValid() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
            uint8 result;
            //Получение указателя на перечисление ECellTypeOfMapStructure для выполнения проверок
            const UEnum* CellType = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECellTypeOfMapStructure"), true);
            //Получение значения из выше выбранной строки по порядковому номеру столбца
            if (LoadStatement->GetColumnValueByIndex(cellCol, result)) {

                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: The value %d was obtained at index %d:%d from the \"Structure %d:%d\" table"), result, cellRow, cellCol, chunkRow, chunkCol);

                //Проверка является ли полученное значение допустимым для преобразования в перечисление ECellTypeOfMapStructure
                if (result >=0 && result < CellType->GetMaxEnumValue()) {
                    ECellTypeOfMapStructure enumResult = (ECellTypeOfMapStructure)result;

                    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: The value %d received at index %d:%d from the  table is valid for conversion to an enumeration"), result, cellRow, cellCol, chunkRow, chunkCol);

                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");

                    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCell function: The value %d in the table \"Structure %d:%d\" at index %d:%d is fully loaded"), result, chunkRow, chunkCol, cellRow, cellCol);

                    return enumResult;
                }
                else {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to cast the value %d to the ECellTypeOfMapStructure data type obtained at index %d:%d from the \"Structure %d:%d\" table - this number is not valid for casting to ECellTypeOfMapStructure"), result, cellRow, cellCol, chunkRow, chunkCol);

                    destroyLoadStatement("GetValueOfMapChunkStructureCell");
                    if (autoClose)
                        mapDataBaseClose("GetValueOfMapChunkStructureCell");
                    return ECellTypeOfMapStructure::Error;
                }
            }
            else {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to load the value at index %d:%d from table \"Structure %d:%d\": %s"), cellRow, cellCol, chunkRow, chunkCol, *mapDataBase->GetLastError());

                destroyLoadStatement("GetValueOfMapChunkStructureCell");
                if (autoClose)
                    mapDataBaseClose("GetValueOfMapChunkStructureCell");
                return ECellTypeOfMapStructure::Error;
            }
        }
        else {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function when trying to execute a LoadStatement on table \"Structure %d:%d\": %s"), chunkRow, chunkCol, *mapDataBase->GetLastError());

            destroyLoadStatement("GetValueOfMapChunkStructureCell");
            if (autoClose)
                mapDataBaseClose("GetValueOfMapChunkStructureCell");
            return ECellTypeOfMapStructure::Error;
        }

        destroyLoadStatement("GetValueOfMapChunkStructureCell");
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetValueOfMapChunkStructureCell function - mapDataBase is not valid"));
        return ECellTypeOfMapStructure::Error;
    }
}

//Функция, закрывающая базу данных
void UMapMatrix::mapDataBaseClose(FString FunctionName)
{
    if (mapDataBase->IsValid()) {
        if (!mapDataBase->Close()) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the %s function when trying to close mapDataBase: %s"), *FunctionName, *mapDataBase->GetLastError());
            return;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the %s function: mapDataBase has been closed"), *FunctionName);
    }
    else {
        UE_LOG(MapDataBase, Warning, TEXT("Warning in MapMatrix class in %s function - An attempt was made to close mapDataBase, but this database was not open"), *FunctionName);
    }
}

/* Функция, записывающая значение в ячейку фрагмента БД по её глобальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
bool UMapMatrix::SetValueOfMapChunkCellByGlobalIndex(MatrixType matrixType, int32 globalCellRow, int32 globalCellCol, int32 value, bool autoClose)
{
    int32 chunkRow;
    int32 cellRow;
    int32 chunkCol;
    int32 cellCol;

    convertingGlobalIndexIntoLocalOne(globalCellRow, globalCellCol, chunkRow, cellRow, chunkCol, cellCol);

    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function: Global index %d:%d translated into tables \"%s %d:%d\" cell %d:%d"), globalCellRow, globalCellCol, *getStringMatrixType(matrixType), chunkRow, chunkCol, cellRow, cellCol);

    /* Если автозакрытие отключено, то база данных открывается здесь с модификатором ReadWriteCreate,
     * который подойдёт и для создания таблиц, и для записи данных в ячейки*/
    if (!autoClose && !mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return false;
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkCellByGlobalIndex function: mapDataBase has been opened"));
    }

    /* Всегда перед попыткой записать данные в ячейку идёт попытка создать необходимую таблицу.
     * Если таблица уже существует, просто ничего не произойдёт, и запись данных продолжится*/
    if (CreateMapChunk(matrixType, chunkRow, chunkCol, autoClose))
        return SetValueOfMapChunkCell(matrixType, chunkRow, chunkCol, cellRow, cellCol, value, autoClose);
    else
        return false;
}

/* Функция, считывающая значение из ячейки фрагмента БД по её глобальному индексу.
 * Стоит быть внимательным при назначении autoClose false - mapDataBase не будет закрыта автоматически*/
ECellTypeOfMapStructure UMapMatrix::GetValueOfMapChunkStructureCellByGlobalIndex(int32 globalCellRow, int32 globalCellCol, bool autoClose)
{
    int32 chunkRow;
    int32 cellRow;
    int32 chunkCol;
    int32 cellCol;

    convertingGlobalIndexIntoLocalOne(globalCellRow, globalCellCol, chunkRow, cellRow, chunkCol, cellCol);

    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetValueOfMapChunkStructureCellByGlobalIndex function: Global index %d:%d translated into tables \"%s %d:%d\" cell %d:%d"), globalCellRow, globalCellCol, *getStringMatrixType(MatrixType::ChunkStructure), chunkRow, chunkCol, cellRow, cellCol);

    return GetValueOfMapChunkStructureCell(chunkRow, chunkCol, cellRow, cellCol, autoClose);
}

//Функция, устанавливающая имя файла с базой данных
void UMapMatrix::SetFileName(FString fileName, bool WithExtension)
{
    FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/") + fileName;
    if (!WithExtension)
        FilePath += TEXT(".db");
    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetFileName function: The name of the database file is set to %s, the path to the file is %s"), *fileName, *FilePath);
}

//Функция, устанавливающая путь до файла с базой данных
void UMapMatrix::SetFilePath(FString filePath)
{
    FilePath = filePath;
    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetFilePath function: The path to the database file is set as %s"), *FilePath);
}

//Функция, запускающая в отдельном потоке создание в базе даннх матрицы из фрагментов карты указанного типа
void UMapMatrix::AsyncCreateBlankCard(int32 rowLen, int32 colLen, MatrixType matrixType) {
    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Started asynchronous creation of %d rows by %d columns of map fragments with fragment type %s"), rowLen+1, colLen+1, *getStringMatrixType(matrixType));
    SuccessCreateBlankCard = false;

    //Запуск асинхронного потока
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [rowLen, colLen, matrixType, this]() {
        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: A thread for creating map fragments has been opened"));
        for (int row = 0; row <= rowLen; row++) {
            for (int col = 0; col <= colLen; col++) {
                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: The creation of the fragment \"%s %d:%d\" has begun"), *getStringMatrixType(matrixType), row, col);
                SuccessCreateBlankCard = CreateMapChunk(matrixType, row, col, false);
                //Если хотябы одна таблица не создалась, то дальнейшее создание таблиц прекращается
                if (!SuccessCreateBlankCard){
                    UE_LOG(MapDataBase, Error, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Fragment \"%s %d:%d\" was not created"), *getStringMatrixType(matrixType), row, col);
                    break;
                }else
                    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Fragment \"%s %d:%d\" has been created"), *getStringMatrixType(matrixType), row, col);
            }
            if (!SuccessCreateBlankCard)
                break;
        }

        mapDataBaseClose("AsyncCreateBlankCard");

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: The creation of all map fragments has been completed"));

        //Следующие функции запускаются в основном потоке так как могут быть выполнены только в нём
        AsyncTask(ENamedThreads::GameThread, [this]() {
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Launched a GameThread call from the map fragments creation thread to switch widgets"));
            
            //Если все таблицы не были успешно созданы, то файл с ними удаляется
            if (!SuccessCreateBlankCard && !FilePath.IsEmpty()) {
                if (FPaths::ValidatePath(FilePath) && FPaths::FileExists(FilePath))
                {
                    IFileManager& FileManager = IFileManager::Get();
                    FileManager.Delete(*FilePath);
                }
            }

            //Виджет загрузки удаляется с экрана, и посылается сигнал окончания создания таблицы
            if (this->DownloadWidget) {
                this->DownloadWidget->RemoveFromParent();
                DownloadWidget->LoadingComplete(SuccessCreateBlankCard);
            }
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: Widget switching done"));
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the AsyncCreateBlankCard function: A thread for creating map fragments has been closed"));
            });
        });
}

FMapDimensions UMapMatrix::GetMapDimensions(bool autoClose)
{
    /* Если mapDataBase непроинициализированна, это означает, что база
     * данных не была открыта. В таком случае её следует открыть.*/
    if (!mapDataBase->IsValid()) {
        if (!mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the GetMapDimensions function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
            return FMapDimensions();
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the GetMapDimensions function: mapDataBase has been opened"));
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
                return FMapDimensions(MinRow, MaxRow, MinCol, MaxCol);
            }
        }
        else
            return FMapDimensions();
    }

    return FMapDimensions();
}

FMapDimensions::FMapDimensions(int32 MinRow, int32 MaxRow, int32 MinCol, int32 MaxCol, bool isValid):
    MinRow(MinRow), MaxRow(MaxRow), MinCol(MinCol), MaxCol(MaxCol), isValid(isValid)
{}

FMapDimensions::FMapDimensions() : FMapDimensions(0, 0, 0, 0, false)
{}
