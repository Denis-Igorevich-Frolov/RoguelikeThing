// Fill out your copyright notice in the Description page of Project Settings.


#include "CoordWrapperOfTable.h"

UWidget* WrapperRow::FindWidget(int Key)
{
    return *Row.Find(Key);
}

void WrapperRow::AddWidget(int Key, UWidget* Widget)
{
    Row.Add(Key, Widget);
}

UCoordWrapperOfTable::~UCoordWrapperOfTable()
{
    for (auto& Elem : Col) {
        if(Elem.Value)
            delete Elem.Value;
    }
}

void UCoordWrapperOfTable::AddWidget(int col, int row, UWidget* Widget)
{
    if (Col.Contains(col)) {
        (*Col.Find(col))->AddWidget(row, Widget);
    }
    else {
        WrapperRow* newRow = new WrapperRow();
        newRow->AddWidget(row, Widget);
        Col.Add(col, newRow);
    }
}

UWidget* UCoordWrapperOfTable::FindWidget(int col, int row)
{
    return  (*Col.Find(col))->FindWidget(row);
}
