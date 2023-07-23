// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMatrix.h"

DEFINE_LOG_CATEGORY(MapDataBase);

UMapMatrix::UMapMatrix()
{
    mapDataBase = new FSQLiteDatabase();
}

UMapMatrix::~UMapMatrix()
{
    if(LoadStatement)
        delete LoadStatement;
    if (mapDataBase->IsValid()) {
        mapDataBase->Close();
        delete mapDataBase;
    }
}

void UMapMatrix::mapDataBaseClose(FString FunctionName)
{
    if (mapDataBase->IsValid()) {
        if (!mapDataBase->Close()) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the %s function when trying to close mapDataBase: %s"), *FunctionName, *mapDataBase->GetLastError());
            return;
        }
    }

    UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the mapDataBaseClose function: mapDataBase has been closed"));
}

void UMapMatrix::Test()
{
    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly);

    if (mapDataBase->IsValid()) {
        LoadStatement->Reset();
        LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM \"Structure 0:0\" WHERE RowNum IS 4;"), ESQLitePreparedStatementFlags::Persistent);
        if (mapDataBase->IsValid() && LoadStatement->IsValid()) {
            if (LoadStatement->Execute() && LoadStatement->Step() == ESQLitePreparedStatementStepResult::Row) {
                uint32 ii;
                if (LoadStatement->GetColumnValueByIndex(1, ii))
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%i"), ii));
                else
                    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("sASSSS"));
            }
            else
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("ASSSS"));
        }
        else {
            UE_LOG(MapDataBase, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
        }
        LoadStatement->Destroy();

        if (!mapDataBase->Close()) {
            UE_LOG(MapDataBase, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
        }
        mapDataBase->Execute(TEXT("COMMIT;"));
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }
}

bool UMapMatrix::CreateMapChunkStructure(int32 chunkRow, int32 chunkCol)
{
    if (mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate) && mapDataBase->IsValid()) {
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            mapDataBaseClose("CreateMapChunkStructure");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: A transaction was started to create table \"Structure %d:%d\""), chunkRow, chunkCol);

        bool TableIsExists = LoadStatement->Create(*mapDataBase, *(FString::Printf(TEXT("SELECT * FROM \"Structure %d:%d\" WHERE RowNum IS %d;"), chunkRow, chunkCol, TableLength)));
        FString QueryToCreateTable = FString::Printf(TEXT(
            "CREATE TABLE IF NOT EXISTS \"Structure %d:%d\"("
            "RowNum INTEGER PRIMARY KEY AUTOINCREMENT,"), chunkRow, chunkCol);

        for (int i = 1; i <= TableLength; i++) {
            QueryToCreateTable += FString::Printf(TEXT("\"Col %d\" INTEGER"), i);
            if(i != TableLength)
                QueryToCreateTable += FString(TEXT(","));
        }

        QueryToCreateTable += FString(TEXT("); "));

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: Generated query to create \"Structure %d:%d\" table with %d columns"), chunkRow, chunkCol, TableLength);

        if (!mapDataBase->Execute(*QueryToCreateTable)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to create the mapDataBase table: %s"), *mapDataBase->GetLastError());
            mapDataBaseClose("CreateMapChunkStructure");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: Query to create table \"Structure %d:%d\" completed"), chunkRow, chunkCol);

        if (!TableIsExists) {

                UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: The table \"Structure %d:%d\" is initialized for the first time, the consciousness of %d rows in the table is started"), chunkRow, chunkCol, TableLength);

            for (int i = 0; i < 51; i++) {
                if (!mapDataBase->Execute(*(FString::Printf(TEXT("INSERT INTO \"Structure %d:%d\" DEFAULT VALUES;"), chunkRow, chunkCol)))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to insert a row into mapDataBase: %s"), *mapDataBase->GetLastError());
                    mapDataBaseClose("CreateMapChunkStructure");
                    return false;
                }
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: The creation of %d rows in the \"Structure %d:%d\" table has been completed"), TableLength, chunkRow, chunkCol);
        }

        if (!LoadStatement->Destroy()) {
            UE_LOG(MapDataBase, Warning, TEXT("Warning in MapMatrix class in CreateMapChunkStructure function - LoadStatement was not destroyed. If this warning appears only once during the initial table initialization, ignore it"));
        }
        else
            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: The LoadStatement object has been destroyed"));

        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            
            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkStructureCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }
            
            mapDataBaseClose("CreateMapChunkStructure");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the CreateMapChunkStructure function: The transaction to create the table \"Structure %d:%d\" has been committed"), chunkRow, chunkCol);
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
        return false;
    }

    mapDataBaseClose("CreateMapChunkStructure");
    return true;
}

bool UMapMatrix::SetValueOfMapChunkStructureCell(int32 chunkRow, int32 chunkCol, int32 cellRow, int32 cellCol, int32 value)
{
    if (cellRow < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkStructureCell function - cellRow value (%d) is less than 1"), cellRow);
        return false;
    }
    if (cellRow > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkStructureCell function - cellRow value (%d) is greater than TableLength (%d)"), cellRow , TableLength);
        return false;
    }
    if (cellCol < 1) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkStructureCell function - cellCol value (%d) is less than 1"), cellCol);
        return false;
    }
    if (cellCol > TableLength) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkStructureCell function - cellCol value (%d) is greater than TableLength (%d)"), cellCol , TableLength);
        return false;
    }

    if (mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWrite) && mapDataBase->IsValid()) {
        if (!mapDataBase->Execute(TEXT("BEGIN;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the UMapMatrix class in the SetValueOfMapChunkStructureCell function when trying to start a mapDataBase transaction: %s"), *mapDataBase->GetLastError());

            mapDataBaseClose("SetValueOfMapChunkStructureCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkStructureCell function: A transaction to write data to table \"Structure %d:%d\" has begun"), chunkRow, chunkCol);

        FString QueryToSetCellValue = FString::Printf(TEXT(
            "UPDATE \"Structure %d:%d\" SET \"Col %d\" = %d WHERE RowNum = %d;"), chunkRow, chunkCol, cellCol, value, cellRow);

        if (!mapDataBase->Execute(*QueryToSetCellValue)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkStructureCell function when trying to set a value in the \"Structure %d:%d\" table to cell %d:%d: %s"), chunkRow, chunkCol, cellRow, cellCol, *mapDataBase->GetLastError());

            mapDataBaseClose("SetValueOfMapChunkStructureCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkStructureCell function: The request to write data to table \"Structure %d:%d\" has been completed"), chunkRow, chunkCol);

        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkStructureCell function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            
            if (!mapDataBase->Execute(TEXT("ROLLBACK;"))) {
                UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkStructureCell function when trying to rollback the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            }

            UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkStructureCell function: The transaction to write data to table \"Structure %d:%d\" was rolled back"), chunkRow, chunkCol);

            mapDataBaseClose("SetValueOfMapChunkStructureCell");
            return false;
        }

        UE_LOG(MapDataBase, Log, TEXT("MapMatrix class in the SetValueOfMapChunkStructureCell function: The transaction to write data to table \"Structure %d:%d\" was committed"), chunkRow, chunkCol);
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the SetValueOfMapChunkStructureCell function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
        return false;
    }

    mapDataBaseClose("SetValueOfMapChunkStructureCell");
    return true;
}
