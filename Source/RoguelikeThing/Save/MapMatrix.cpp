// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Save/MapMatrix.h"

FMapCellStructure UMapMatrix::GetCell(UPARAM()int32 row, UPARAM()int32 col)
{
    if (map.IsValidIndex(row)) {
        if (map[row].row.IsValidIndex(col))
            return map[col].row[row];
        else {
            UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the GetCell function: Index out of range"));
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                    "FMapCellStructure gave an error in the GetCell function: Index out of range"));
            return FMapCellStructure();
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the GetCell function: Index out of range"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the GetCell function: Index out of range"));
        return FMapCellStructure();
    }
}

int32 UMapMatrix::GetRowLength()
{
    return  map.Num();
}

int32 UMapMatrix::GetColLength()
{
    if (map.IsValidIndex(0))
        return map[0].row.Num();
    else
        return 0;
}

void UMapMatrix::addRow()
{
    FMapRow newRow;
    for (int i = 0; i < GetColLength(); i++) {
        FMapCellStructure newCell;
        newCell.isValid = true;
        newRow.row.Emplace(newCell);
    }

    map.Emplace(newRow);
}

void UMapMatrix::addCol()
{
    for (FMapRow& row : map)
    {
        FMapCellStructure newCell;
        newCell.isValid = true;
        row.row.Emplace(newCell);
    }
}

void UMapMatrix::addMultipleRows(UPARAM() int32 quantity) {
    for (int i = 0; i < quantity; i++)
        addRow();
}

void UMapMatrix::addMultipleCols(UPARAM() int32 quantity) {
    for (int i = 0; i < quantity; i++)
        addCol();
}

void UMapMatrix::stretchRows(UPARAM()int32 stretchTo)
{
    int numberOfNewRows = stretchTo - map.Num();

    if (numberOfNewRows >= 0) {
        addMultipleRows(numberOfNewRows);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the stretchRows function: The passed rows size is less than to the current size"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the stretchRows function: The passed rows size is less than to the current size"));
    }
}

void UMapMatrix::stretchCols(UPARAM()int32 stretchTo)
{
    int numberOfNewCols;
    if (map.IsValidIndex(0))
        numberOfNewCols = stretchTo - map[0].row.Num();
    else
        numberOfNewCols = stretchTo;

    if (numberOfNewCols >= 0) {
        addMultipleCols(numberOfNewCols);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the stretchCols function: The passed columns size is less than to the current size"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the stretchCols function: The passed columns size is less than to the current size"));
    }
}

void UMapMatrix::stretchMap(UPARAM()int32 rowsStretchTo, UPARAM()int32 colsStretchTo)
{
    stretchRows(rowsStretchTo);
    stretchCols(colsStretchTo);

}

void UMapMatrix::addRowToBeginning()
{
    FMapRow newRow;
    for (int i = 0; i < GetColLength(); i++) {
        FMapCellStructure newCell;
        newCell.isValid = true;
        newRow.row.Emplace(newCell);
    }

    map.Insert(newRow, 0);
}

void UMapMatrix::addColToBeginning()
{
    for (FMapRow& row : map)
    {
        FMapCellStructure newCell;
        newCell.isValid = true;
        row.row.Insert(newCell, 0);
    }
}

void UMapMatrix::addMultipleRowsToBeginning(UPARAM()int32 quantity)
{
    for (int i = 0; i < quantity; i++)
        addRowToBeginning();
}

void UMapMatrix::addMultipleColsToBeginning(UPARAM()int32 quantity)
{
    for (int i = 0; i < quantity; i++)
        addColToBeginning();
}

void UMapMatrix::stretchRowsAtBeginning(UPARAM()int32 stretchTo)
{
    int numberOfNewRows = stretchTo - map.Num();

    if (numberOfNewRows >= 0) {
        addMultipleRowsToBeginning(numberOfNewRows);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the stretchRowsAtBeginning function: The passed rows size is less than to the current size"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the stretchRowsAtBeginning function: The passed rows size is less than to the current size"));
    }
}

void UMapMatrix::stretchColsAtBeginning(UPARAM()int32 stretchTo)
{
    int numberOfNewCols;
    if (map.IsValidIndex(0))
        numberOfNewCols = stretchTo - map[0].row.Num();
    else
        numberOfNewCols = stretchTo;

    if (numberOfNewCols >= 0) {
        addMultipleColsToBeginning(numberOfNewCols);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the stretchColsAtBeginning function: The passed columns size is less than to the current size"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the stretchColsAtBeginning function: The passed columns size is less than to the current size"));
    }
}

void UMapMatrix::stretchMapAtBeginning(UPARAM()int32 rowsStretchTo, UPARAM()int32 colsStretchTo)
{
    stretchRowsAtBeginning(rowsStretchTo);
    stretchColsAtBeginning(colsStretchTo);
}
