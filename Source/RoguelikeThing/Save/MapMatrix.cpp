// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Save/MapMatrix.h"

void UMapMatrix::Test()
{
    db->Open (TEXT("E:\\test.db"), ESQLiteDatabaseOpenMode::ReadWrite);
    db->Execute(TEXT("CREATE TABLE \"test2\" ("
        "\"Row\"	INTEGER"
        "); "));
    db->Close();
}