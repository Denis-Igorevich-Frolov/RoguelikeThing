// Denis Igorevich Frolov did all this. Once there. All things reserved.


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
    bool success = false;
    if (Col.Contains(col)) {
        //Если строка существует в неё добавляется новый элемент
        WrapperRow* Row = *Col.Find(col);

        if (Row) {
            bool WidgetNotContains = !Row->Contains(row);
            success = (bool)Row->AddWidget(row, Widget);

            if (WidgetNotContains && success)
                NumberOfItemsInTable++;
        }
    }
    else {
        //Если строки не существует, она создаётся и в неё добавляется новый элемент
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
        //Если минимальная координата не существует, значит матрица была пуста, и текущий элемент единственный и минимальный
        if (!MinCoord.getIsInit())
            MinCoord = FGridCoord(row, col);
        else {
            if (col < MinCoord.Col)
                MinCoord.Col = col;
            if (row < MinCoord.Row)
                MinCoord.Row = row;
        }

        //Если максимальная координата не существует, значит матрица была пуста, и текущий элемент единственный и максимальный
        if (!MaxCoord.getIsInit())
            MaxCoord = FGridCoord(row, col);
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
            //Проверяется есть ли в существующих столбцах хотя бы одна строка с хотя бы одним элементом
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

//Получение самой левой нижней координаты
FGridCoord UCoordWrapperOfTable::getMinCoord()
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

    MinCoord = FGridCoord();
    MaxCoord = FGridCoord();
    NumberOfItemsInTable = 0;
}

//Получение самой правой верхней координаты
FGridCoord UCoordWrapperOfTable::getMaxCoord()
{
    return MaxCoord;
}

FGridCoord::FGridCoord() : Row(-1), Col(-1), isInit(false)
{}

FGridCoord::FGridCoord(int row, int col) : Row(row), Col(col), isInit(true)
{}

//Проверка проинициализированности координаты
bool FGridCoord::getIsInit()
{
    return isInit;
}


FString FGridCoord::ToString()
{
    return FString("Row: " + FString::FromInt(Row) + " Col: " + FString::FromInt(Col));
}

bool FGridCoord::operator==(const FGridCoord& Coord) const
{
    return Row == Coord.Row && Col == Coord.Col;
}

bool FGridCoord::operator!=(const FGridCoord& Coord) const
{
    return Row != Coord.Row || Col != Coord.Col;
}

FGridCoord FGridCoord::operator+(const FGridCoord Bias) const
{
    return FGridCoord(Row + Bias.Row, Col + Bias.Col);
}

FGridCoord FGridCoord::operator-(const FGridCoord Bias) const
{
    return FGridCoord(Row - Bias.Row, Col - Bias.Col);
}


FGridDimensions::FGridDimensions(FGridCoord min, FGridCoord max) : Min(min), Max(max)
{}

FString FGridDimensions::ToString()
{
    return FString("Min(" + Min.ToString() + "), Max(" + Max.ToString() + ")");
}

bool FGridDimensions::IsEmpty()
{
    return Min.Row == 0 && Min.Col == 0 && Max.Row == 0 && Max.Col == 0;
}

bool FGridDimensions::operator==(const FGridDimensions& Dimensions) const
{
    return Min == Dimensions.Min && Max == Dimensions.Max;
}

bool FGridDimensions::operator!=(const FGridDimensions& Dimensions) const
{
    return Min != Dimensions.Min || Max != Dimensions.Max;
}

FGridDimensions FGridDimensions::operator+(const FGridDimensions Bias) const
{
    return FGridDimensions(Min + Bias.Min, Max + Bias.Max);
}

FGridDimensions FGridDimensions::operator-(const FGridDimensions Bias) const
{
    return FGridDimensions(Min - Bias.Min, Max - Bias.Max);
}