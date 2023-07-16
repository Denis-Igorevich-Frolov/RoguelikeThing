// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMatrix.h"

UMapMatrix::UMapMatrix()
{
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
    mapDataBase = new FSQLiteDatabase();
    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate);

    if (mapDataBase->IsValid()) {
        mapDataBase->Execute(TEXT("BEGIN;"));

        bool TableIsExists = LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM \"Chunk 0:0\" WHERE id IS 51;"));

        mapDataBase->Execute(TEXT(
            "CREATE TABLE IF NOT EXISTS \"Chunk 0:0\"("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "\"Col 1\"	INTEGER DEFAULT 11,"
            "\"Col 2\"	INTEGER DEFAULT 22,"
            "\"Col 3\"	INTEGER DEFAULT 33,"
            "\"Col 4\"	INTEGER DEFAULT 44,"
            "\"Col 5\"	INTEGER DEFAULT 55,"
            "\"Col 6\"	INTEGER DEFAULT 66,"
            "\"Col 7\"	INTEGER DEFAULT 32,"
            "\"Col 8\"	INTEGER DEFAULT 32,"
            "\"Col 9\"	INTEGER DEFAULT 32,"
            "\"Col 10\"	INTEGER DEFAULT 32,"
            "\"Col 11\"	INTEGER DEFAULT 32,"
            "\"Col 12\"	INTEGER DEFAULT 32,"
            "\"Col 13\"	INTEGER DEFAULT 32,"
            "\"Col 14\"	INTEGER DEFAULT 32,"
            "\"Col 15\"	INTEGER DEFAULT 32,"
            "\"Col 16\"	INTEGER DEFAULT 32,"
            "\"Col 17\"	INTEGER DEFAULT 32,"
            "\"Col 18\"	INTEGER DEFAULT 32,"
            "\"Col 19\"	INTEGER DEFAULT 32,"
            "\"Col 20\"	INTEGER DEFAULT 32,"
            "\"Col 21\"	INTEGER DEFAULT 32,"
            "\"Col 22\"	INTEGER DEFAULT 32,"
            "\"Col 23\"	INTEGER DEFAULT 32,"
            "\"Col 24\"	INTEGER DEFAULT 32,"
            "\"Col 25\"	INTEGER DEFAULT 32,"
            "\"Col 26\"	INTEGER DEFAULT 32,"
            "\"Col 27\"	INTEGER DEFAULT 32,"
            "\"Col 28\"	INTEGER DEFAULT 32,"
            "\"Col 29\"	INTEGER DEFAULT 32,"
            "\"Col 30\"	INTEGER DEFAULT 32,"
            "\"Col 31\"	INTEGER DEFAULT 32,"
            "\"Col 32\"	INTEGER DEFAULT 32,"
            "\"Col 33\"	INTEGER DEFAULT 32,"
            "\"Col 34\"	INTEGER DEFAULT 32,"
            "\"Col 35\"	INTEGER DEFAULT 32,"
            "\"Col 36\"	INTEGER DEFAULT 32,"
            "\"Col 37\"	INTEGER DEFAULT 32,"
            "\"Col 38\"	INTEGER DEFAULT 32,"
            "\"Col 39\"	INTEGER DEFAULT 32,"
            "\"Col 40\"	INTEGER DEFAULT 32,"
            "\"Col 41\"	INTEGER DEFAULT 32,"
            "\"Col 42\"	INTEGER DEFAULT 32,"
            "\"Col 43\"	INTEGER DEFAULT 32,"
            "\"Col 44\"	INTEGER DEFAULT 32,"
            "\"Col 45\"	INTEGER DEFAULT 32,"
            "\"Col 46\"	INTEGER DEFAULT 32,"
            "\"Col 47\"	INTEGER DEFAULT 32,"
            "\"Col 48\"	INTEGER DEFAULT 32,"
            "\"Col 49\"	INTEGER DEFAULT 32,"
            "\"Col 50\"	INTEGER DEFAULT 32,"
            "\"Col 51\"	INTEGER DEFAULT 32"
            "); "));

        if (!TableIsExists) {
            for (int i = 0; i < 51; i++)
                mapDataBase->Execute(TEXT("INSERT INTO \"Chunk 0:0\" DEFAULT VALUES;"));
        }
        LoadStatement->Destroy();

        mapDataBase->Execute(TEXT("COMMIT;"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }

    if (!mapDataBase->Close()) {
        UE_LOG(LogTemp, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
    }

    /////////////////////////

    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWrite);

    if (mapDataBase->IsValid()) {
        mapDataBase->Execute(TEXT(
            "UPDATE \"Chunk 0:0\" SET \"Col 1\" = 234 WHERE id = 4;"
            "COMMIT;"));

        if (!mapDataBase->Close()) {
            UE_LOG(LogTemp, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }

    /////////////////////////

    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadOnly);

    if (mapDataBase->IsValid()) {
        LoadStatement->Reset();
        LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM \"Chunk 0:0\" WHERE id IS 4;"), ESQLitePreparedStatementFlags::Persistent);
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
            UE_LOG(LogTemp, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
        }
        LoadStatement->Destroy();

        if (!mapDataBase->Close()) {
            UE_LOG(LogTemp, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
        }
        mapDataBase->Execute(TEXT("COMMIT;"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }
}