// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Save/MapMatrix.h"

UMapMatrix::UMapMatrix()
{
    InvalidEmptyCell.isValid = false;
}

const FMapCellStructure& UMapMatrix::GetCell(int32 row, int32 col)
{
    if (map.IsValidIndex(row)) {
        if (map[row].row.IsValidIndex(col))
            return map[col].row[row];
        else {
            UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the GetCell function: Index out of range"));
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                    "FMapCellStructure gave an error in the GetCell function: Index out of range"));
            return InvalidEmptyCell;
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the GetCell function: Index out of range"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the GetCell function: Index out of range"));
        return InvalidEmptyCell;
    }
}

bool UMapMatrix::SetCell(FMapCellStructure Cell, int32 row, int32 col)
{
    if (map.IsValidIndex(row)) {
        if (map[row].row.IsValidIndex(col)) {
            map[col].row[row] = Cell;
            return true;
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the SetCell function: Index out of range"));
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                    "FMapCellStructure gave an error in the SetCell function: Index out of range"));
            return false;
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("FMapCellStructure gave an error in the SetCell function: Index out of range"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the SetCell function: Index out of range"));
        return false;
    }
}

const TArray<FMapRow>& UMapMatrix::GetMap()
{
    return map;
}

void UMapMatrix::SetMap(UPARAM(ref) TArray<FMapRow>& newMap)
{
    map = newMap;
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
        newRow.row.Emplace(newCell);
    }

    map.Emplace(newRow);
}

void UMapMatrix::addCol()
{
    for (FMapRow& row : map)
    {
        FMapCellStructure newCell;
        row.row.Emplace(newCell);
    }
}

void UMapMatrix::addMultipleRows(int32 quantity) {
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the addMultipleRows function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the addMultipleRows function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        addRow();
}

void UMapMatrix::addMultipleCols(int32 quantity) {
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the addMultipleCols function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the addMultipleCols function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        addCol();
}

void UMapMatrix::enlargeRows(int32 enlargeTo)
{
    if (enlargeTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the enlargeRows function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the enlargeRows function: Passed value is negative"));
        return;
    }

    int numberOfNewRows = enlargeTo - map.Num();

    if (numberOfNewRows >= 0) {
        addMultipleRows(numberOfNewRows);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the enlargeRows function: The passed rows size is less than to the current size. Map will be no enlarge"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the enlargeRows function: The passed rows size is less than to the current size. Map will be no enlarge"));
    }
}

void UMapMatrix::enlargeCols(int32 enlargeTo)
{
    if (enlargeTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the enlargeCols function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the enlargeCols function: Passed value is negative"));
        return;
    }

    int numberOfNewCols;
    if (map.IsValidIndex(0))
        numberOfNewCols = enlargeTo - map[0].row.Num();
    else
        numberOfNewCols = enlargeTo;

    if (numberOfNewCols >= 0) {
        addMultipleCols(numberOfNewCols);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the enlargeCols function: The passed columns size is less than to the current size. Map will be no enlarge"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the enlargeCols function: The passed columns size is less than to the current size. Map will be no enlarge"));
    }
}

void UMapMatrix::enlargeMap(int32 enlargeRowsTo, int32 enlargeColsTo)
{
    enlargeRows(enlargeRowsTo);
    enlargeCols(enlargeColsTo);

}

void UMapMatrix::addRowToBeginning()
{
    FMapRow newRow;
    for (int i = 0; i < GetColLength(); i++) {
        FMapCellStructure newCell;
        newRow.row.Emplace(newCell);
    }

    map.Insert(newRow, 0);
}

void UMapMatrix::addColToBeginning()
{
    for (FMapRow& row : map)
    {
        FMapCellStructure newCell;
        row.row.Insert(newCell, 0);
    }
}

void UMapMatrix::addMultipleRowsToBeginning(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the addMultipleRowsToBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the addMultipleRowsToBeginning function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        addRowToBeginning();
}

void UMapMatrix::addMultipleColsToBeginning(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the addMultipleColsToBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the addMultipleColsToBeginning function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        addColToBeginning();
}

void UMapMatrix::enlargeRowsAtBeginning(int32 enlargeTo)
{
    if (enlargeTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the enlargeRowsAtBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the enlargeRowsAtBeginning function: Passed value is negative"));
        return;
    }

    int numberOfNewRows = enlargeTo - map.Num();

    if (numberOfNewRows >= 0) {
        addMultipleRowsToBeginning(numberOfNewRows);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the enlargeRowsAtBeginning function: The passed rows size is less than to the current size. Map will be no enlarge"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the enlargeRowsAtBeginning function: The passed rows size is less than to the current size. Map will be no enlarge"));
    }
}

void UMapMatrix::enlargeColsAtBeginning(int32 enlargeTo)
{
    if (enlargeTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the enlargeColsAtBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the enlargeColsAtBeginning function: Passed value is negative"));
        return;
    }

    int numberOfNewCols;
    if (map.IsValidIndex(0))
        numberOfNewCols = enlargeTo - map[0].row.Num();
    else
        numberOfNewCols = enlargeTo;

    if (numberOfNewCols >= 0) {
        addMultipleColsToBeginning(numberOfNewCols);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the enlargeColsAtBeginning function: The passed columns size is less than to the current size. Map will be no enlarge"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the enlargeColsAtBeginning function: The passed columns size is less than to the current size. Map will be no enlarge"));
    }
}

void UMapMatrix::enlargeMapAtBeginning(int32 enlargeRowsTo, int32 enlargeColsTo)
{
    enlargeRowsAtBeginning(enlargeRowsTo);
    enlargeColsAtBeginning(enlargeColsTo);
}

void UMapMatrix::deleteLastRow()
{
    if (map.Num())
        map.RemoveAt(map.Num()-1);
}

void UMapMatrix::deleteLastCol()
{
    for (FMapRow& row : map)
    {
        if(row.row.Num())
            row.row.RemoveAt(row.row.Num() - 1);
    }
}

void UMapMatrix::deleteMultipleLastRows(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the deleteMultipleLastRows function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the deleteMultipleLastRows function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        deleteLastRow();
}

void UMapMatrix::deleteMultipleLastCols(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the deleteMultipleLastCols function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the deleteMultipleLastCols function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        deleteLastCol();
}

void UMapMatrix::truncateRowsFromEnd(int32 truncateTo)
{
    if (truncateTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the truncateRowsFromEnd function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the truncateRowsFromEnd function: Passed value is negative"));
        return;
    }

    int numberOfTruncatedRows = map.Num() - truncateTo;

    if (numberOfTruncatedRows < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the truncateRowsFromEnd function: The passed rows size is less than to the current size. Truncation will not occur"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the truncateRowsFromEnd function: The passed rows size is less than to the current size. Truncation will not occur"));
    
        return;
    }

    if (numberOfTruncatedRows <= map.Num()) {
        deleteMultipleLastRows(numberOfTruncatedRows);
    }
}

void UMapMatrix::truncateColsFromEnd(int32 truncateTo)
{
    if (truncateTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the truncateColsFromEnd function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the truncateColsFromEnd function: Passed value is negative"));
        return;
    }

    int numberOfTruncatedCols = GetColLength() - truncateTo;

    if (numberOfTruncatedCols < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the truncateColsFromEnd function: The passed cols size is less than to the current size. Truncation will not occur"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the truncateColsFromEnd function: The passed cols size is less than to the current size. Truncation will not occur"));

        return;
    }

    if (numberOfTruncatedCols <= GetColLength()) {
        deleteMultipleLastCols(numberOfTruncatedCols);
    }
}

void UMapMatrix::truncateMapFromEnd(int32 truncateRowsTo, int32 truncateColsTo)
{
    truncateRowsFromEnd(truncateRowsTo);
    truncateColsFromEnd(truncateColsTo);
}

void UMapMatrix::deleteFirstRow()
{
    if (map.IsValidIndex(0))
        map.RemoveAt(0);
}

void UMapMatrix::deleteFirstCol()
{
    for (FMapRow& row : map)
    {
        if (row.row.IsValidIndex(0))
            row.row.RemoveAt(0);
    }
}

void UMapMatrix::deleteMultipleFirstRows(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the deleteMultipleFirstRows function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the deleteMultipleFirstRows function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        deleteFirstRow();
}

void UMapMatrix::deleteMultipleFirstCols(int32 quantity)
{
    if (quantity < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the deleteMultipleFirstCols function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the deleteMultipleFirstCols function: Passed value is negative"));
        return;
    }

    for (int i = 0; i < quantity; i++)
        deleteFirstCol();
}

void UMapMatrix::truncateRowsFromBeginning(int32 truncateTo)
{
    if (truncateTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the truncateRowsFromBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the truncateRowsFromBeginning function: Passed value is negative"));
        return;
    }

    int numberOfTruncatedRows = map.Num() - truncateTo;

    if (numberOfTruncatedRows < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the truncateRowsFromBeginning function: The passed rows size is less than to the current size. Truncation will not occur"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the truncateRowsFromBeginning function: The passed rows size is less than to the current size. Truncation will not occur"));

        return;
    }

    if (numberOfTruncatedRows <= map.Num()) {
        deleteMultipleFirstRows(numberOfTruncatedRows);
    }
}

void UMapMatrix::truncateColsFromBeginning(int32 truncateTo)
{
    if (truncateTo < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an error in the truncateColsFromBeginning function: Passed value is negative"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT(
                "FMapCellStructure gave an error in the truncateColsFromBeginning function: Passed value is negative"));
        return;
    }

    int numberOfTruncatedCols = GetColLength() - truncateTo;

    if (numberOfTruncatedCols < 0) {
        UE_LOG(LogTemp, Error, TEXT(
            "FMapCellStructure gave an warning in the truncateColsFromBeginning function: The passed cols size is less than to the current size. Truncation will not occur"));
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT(
                "FMapCellStructure gave an warning in the truncateColsFromBeginning function: The passed cols size is less than to the current size. Truncation will not occur"));

        return;
    }

    if (numberOfTruncatedCols <= GetColLength()) {
        deleteMultipleFirstCols(numberOfTruncatedCols);
    }
}

void UMapMatrix::truncateMapFromBeginning(int32 truncateRowsTo, int32 truncateColsTo)
{
    truncateRowsFromBeginning(truncateRowsTo);
    truncateColsFromBeginning(truncateColsTo);
}
