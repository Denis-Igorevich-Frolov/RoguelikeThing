// Fill out your copyright notice in the Description page of Project Settings.


#include "MapMatrix.h"

UMapMatrix::UMapMatrix()
{
}

UMapMatrix::~UMapMatrix()
{
    if (!mapDataBase->Close()) {
        UE_LOG(LogTemp, Error, TEXT("Faled to close mapDataBase: %s"), *mapDataBase->GetLastError());
    }
    else {
        delete mapDataBase;
    }
}

void UMapMatrix::Test()
{
    mapDataBase->Open(*FilePath, ESQLiteDatabaseOpenMode::ReadWriteCreate);

    if (mapDataBase->IsValid()) {
        mapDataBase->Execute(TEXT("BEGIN;"));

        bool TableIsExists = LoadStatement->Create(*mapDataBase, TEXT("SELECT * FROM \"Chunk 0:0\" WHERE id IS 51;"));

        mapDataBase->Execute(TEXT(
            "CREATE TABLE IF NOT EXISTS \"Chunk 0:0\"("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "\"Col 0\"	INTEGER,"
            "\"Col 1\"	INTEGER,"
            "\"Col 2\"	INTEGER,"
            "\"Col 3\"	INTEGER,"
            "\"Col 4\"	INTEGER,"
            "\"Col 5\"	INTEGER,"
            "\"Col 6\"	INTEGER,"
            "\"Col 7\"	INTEGER,"
            "\"Col 8\"	INTEGER,"
            "\"Col 9\"	INTEGER,"
            "\"Col 10\"	INTEGER,"
            "\"Col 11\"	INTEGER,"
            "\"Col 12\"	INTEGER,"
            "\"Col 13\"	INTEGER,"
            "\"Col 14\"	INTEGER,"
            "\"Col 15\"	INTEGER,"
            "\"Col 16\"	INTEGER,"
            "\"Col 17\"	INTEGER,"
            "\"Col 18\"	INTEGER,"
            "\"Col 19\"	INTEGER,"
            "\"Col 20\"	INTEGER,"
            "\"Col 21\"	INTEGER,"
            "\"Col 22\"	INTEGER,"
            "\"Col 23\"	INTEGER,"
            "\"Col 24\"	INTEGER,"
            "\"Col 25\"	INTEGER,"
            "\"Col 26\"	INTEGER,"
            "\"Col 27\"	INTEGER,"
            "\"Col 28\"	INTEGER,"
            "\"Col 29\"	INTEGER,"
            "\"Col 30\"	INTEGER,"
            "\"Col 31\"	INTEGER,"
            "\"Col 32\"	INTEGER,"
            "\"Col 33\"	INTEGER,"
            "\"Col 34\"	INTEGER,"
            "\"Col 35\"	INTEGER,"
            "\"Col 36\"	INTEGER,"
            "\"Col 37\"	INTEGER,"
            "\"Col 38\"	INTEGER,"
            "\"Col 39\"	INTEGER,"
            "\"Col 40\"	INTEGER,"
            "\"Col 41\"	INTEGER,"
            "\"Col 42\"	INTEGER,"
            "\"Col 43\"	INTEGER,"
            "\"Col 44\"	INTEGER,"
            "\"Col 45\"	INTEGER,"
            "\"Col 46\"	INTEGER,"
            "\"Col 47\"	INTEGER,"
            "\"Col 48\"	INTEGER,"
            "\"Col 49\"	INTEGER,"
            "\"Col 50\"	INTEGER"
            "); "));

        if (!TableIsExists) {
            for (int i = 0; i < 51; i++)
                mapDataBase->Execute(TEXT("INSERT INTO \"Chunk 0:0\" DEFAULT VALUES;"));
            LoadStatement->Destroy();
        }

        mapDataBase->Execute(TEXT("COMMIT;"));
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Faled to open mapDataBase: %s"), *mapDataBase->GetLastError());
    }


    LoadStatement->Reset();
    mapDataBase->Execute(TEXT("BEGIN;"));
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

    mapDataBase->Execute(TEXT("COMMIT;"));
    LoadStatement->Destroy();



    mapDataBase->Close();
}