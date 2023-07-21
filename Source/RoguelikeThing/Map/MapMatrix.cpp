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
    if (mapDataBase)
        delete mapDataBase;
}

void UMapMatrix::Test()
{
    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWrite);

    if (mapDataBase->IsValid()) {
        mapDataBase->Execute(TEXT(
            "UPDATE \"Structure 0:0\" SET \"Col 1\" = 234 WHERE RowNum = 4;"
            "COMMIT;"));

        if (!mapDataBase->Close()) {
            UE_LOG(MapDataBase, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
        }
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }

    /////////////////////////

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
            return false;
        }

        bool TableIsExists = LoadStatement->Create(*mapDataBase, *(FString::Printf(TEXT("SELECT * FROM \"Structure %d:%d\" WHERE RowNum IS %d;"), chunkRow, chunkCol, NumberOfRows)));
        FString QueryToCreateTable = FString::Printf(TEXT(
            "CREATE TABLE IF NOT EXISTS \"Structure %d:%d\"("
            "RowNum INTEGER PRIMARY KEY AUTOINCREMENT,"), chunkRow, chunkCol);

        for (int i = 1; i <= NumberOfRows; i++) {
            QueryToCreateTable += FString::Printf(TEXT("\"Col %d\" INTEGER"), i);
            if(i != NumberOfRows)
                QueryToCreateTable += FString(TEXT(","));
        }

        QueryToCreateTable += FString(TEXT("); "));

        if (!mapDataBase->Execute(*QueryToCreateTable)) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to create the mapDataBase table: %s"), *mapDataBase->GetLastError());
            return false;
        }

        if (!TableIsExists) {
            for (int i = 0; i < 51; i++) {
                if (!mapDataBase->Execute(*(FString::Printf(TEXT("INSERT INTO \"Structure %d:%d\" DEFAULT VALUES;"), chunkRow, chunkCol)))) {
                    UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to insert a row into mapDataBase: %s"), *mapDataBase->GetLastError());
                    return false;
                }
            }
        }

        if (!LoadStatement->Destroy())
            UE_LOG(MapDataBase, Warning, TEXT("!!! Warning in MapMatrix class in CreateMapChunkStructure function - LoadStatement was not destroyed"));

        if (!mapDataBase->Execute(TEXT("COMMIT;"))) {
            UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to commit the mapDataBase transaction: %s"), *mapDataBase->GetLastError());
            return false;
        }
    }
    else {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to open mapDataBase: %s"), *mapDataBase->GetLastError());
        return false;
    }

    if (!mapDataBase->Close()) {
        UE_LOG(MapDataBase, Error, TEXT("!!! An error occurred in the MapMatrix class in the CreateMapChunkStructure function when trying to close mapDataBase: %s"), *mapDataBase->GetLastError());
        return false;
    }

    return true;
}
