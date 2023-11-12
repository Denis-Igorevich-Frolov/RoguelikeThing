// Fill out your copyright notice in the Description page of Project Settings.


#include "CoordWrapperOfTable.h"

UWidget* WrapperRow::FindWidget(int Key)
{
    return *Row.Find(Key);
}

bool WrapperRow::AddWidget(int Key, UWidget* Widget)
{
    return (bool)Row.Add(Key, Widget);
}

bool WrapperRow::HasAnyEllements()
{
    return Row.Num() != 0;
}

bool WrapperRow::Contains(int Key)
{
    return Row.Contains(Key);
}

UCoordWrapperOfTable::~UCoordWrapperOfTable()
{
    Clear();
}

bool UCoordWrapperOfTable::AddWidget(int row, int col, UWidget* Widget)
{
    UE_LOG(LogTemp, Error, TEXT("row: %d, col: %d"), row, col);
    bool success = false;
    if (Col.Contains(col)) {
        WrapperRow* Row = *Col.Find(col);

        if (Row) {
            bool WidgetNotContains = !Row->Contains(row);
            success = (bool)Row->AddWidget(row, Widget);

            if (WidgetNotContains && success)
                NumberOfItemsInTable++;
        }
    }
    else {
        WrapperRow* newRow = new WrapperRow();

        if (newRow) {
            success = newRow->AddWidget(row, Widget);
            if (!success) {
                delete newRow;
                return false;
            }

            success = (bool)Col.Add(col, newRow);

            if (success)
                NumberOfItemsInTable++;
        }
    }

    if (success) {
        if (!MinCoord.getIsInit())
            MinCoord = GridCoord(row, col);
        else {
            if (col < MinCoord.Col)
                MinCoord.Col = col;
            if (row < MinCoord.Row)
                MinCoord.Row = row;
        }

        if (!MaxCoord.getIsInit())
            MaxCoord = GridCoord(row, col);
        else {
            if (col > MaxCoord.Col)
                MaxCoord.Col = col;
            if (row > MaxCoord.Row)
                MaxCoord.Row = row;
        }
    }

    return success;
}

UWidget* UCoordWrapperOfTable::FindWidget(int row, int col)
{
    if (!Col.Contains(col))
        return nullptr;
    WrapperRow* Row = *Col.Find(col);
    if(!Row->Contains(row))
        return nullptr;

    return  Row->FindWidget(row);
}

bool UCoordWrapperOfTable::HasAnyEllements()
{
    if(Col.Num() == 0)
        return false;
    else {
        TArray<int32> Keys;
        Col.GetKeys(Keys);
        for (int32 k : Keys)
        {
            if ((*Col.Find(k))->HasAnyEllements())
                return true;
        }

        return false;
    }
}

const int UCoordWrapperOfTable::getNumberOfItemsInTable()
{
    return NumberOfItemsInTable;
}

GridCoord UCoordWrapperOfTable::getMinCoord()
{
    return MinCoord;
}

void UCoordWrapperOfTable::Clear()
{
    for (auto& Elem : Col) {
        if (Elem.Value)
            delete Elem.Value;
    }
    Col.Empty();

    MinCoord = GridCoord();
    MaxCoord = GridCoord();
    NumberOfItemsInTable = 0;
}

GridCoord UCoordWrapperOfTable::getMaxCoord()
{
    return MaxCoord;
}

GridCoord::GridCoord() : Row(-1), Col(-1), isInit(false)
{}

GridCoord::GridCoord(int row, int col) : Row(row), Col(col), isInit(true)
{}

bool GridCoord::getIsInit()
{
    return isInit;
}
