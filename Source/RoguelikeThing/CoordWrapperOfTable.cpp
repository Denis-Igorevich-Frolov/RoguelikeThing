// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "CoordWrapperOfTable.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(CoordWrapperOfTable);

UAbstractTile* UWrapperRow::FindWidget(int Key)
{
    return *Row.Find(Key);
}

UUniformGridSlot* UWrapperRow::FindGridSlot(int Key)
{
    return *RowGrid.Find(Key);
}

//Функция, удаляющая эллемент по переданному ключу
bool UWrapperRow::RemoveWidget(int Key)
{
    UAbstractTile* AbstractTile = FindWidget(Key);

    if (AbstractTile) {
        AbstractTile->RemoveAllCells();
        AbstractTile->ConditionalBeginDestroy();
        AbstractTile->MarkPendingKill();
        AbstractTile->RemoveFromParent();
    }
    else {
        UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the CoordWrapperOfTable class in the RemoveWidget function: AbstractTile is not valid"));
        return false;
    }

    return RemoveIndex(Key);
}

//Функция, исключающая эллемент из координатной обёртки, но не удаляющая его из мира
bool UWrapperRow::RemoveIndex(int Key)
{
    return ((bool)Row.Remove(Key)) && ((bool)RowGrid.Remove(Key));
}

bool UWrapperRow::AddWidget(int Key, UAbstractTile* Widget, UUniformGridSlot* GridSlot)
{
    return ((bool)Row.Add(Key, Widget)) && ((bool)RowGrid.Add(Key, GridSlot));
}

bool UWrapperRow::HasAnyEllements()
{
    return Row.Num() != 0;
}

bool UWrapperRow::Contains(int Key)
{
    return Row.Contains(Key);
}

void UWrapperRow::Clear()
{
    TArray<UAbstractTile*> Tiles;
    Row.GenerateValueArray(Tiles);

    for (UAbstractTile* Tile : Tiles) {
        if (Tile && Tile->IsValidLowLevel()) {
            Tile->RemoveAllCells();
            Tile->ConditionalBeginDestroy();
            Tile->MarkPendingKill();
        }
    }

    Tiles.Empty();
    Row.Empty();
    RowGrid.Empty();
}

UCoordWrapperOfTable::UCoordWrapperOfTable(const FObjectInitializer& Object)
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(CoordWrapperOfTable, Warning, TEXT("Warning in CoordWrapperOfTable class in constructor - GameInstance was not retrieved from the world"));
}

bool UCoordWrapperOfTable::AddWidget(int row, int col, UAbstractTile* Widget, UUniformGridSlot* GridSlot)
{
    if (!Widget) {
        UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the CoordWrapperOfTable class in the AddWidget function: Widget is not valid"));
        return false;
    }

    bool success = false;
    if (ColArr.Contains(col)) {
        //Если строка существует в неё добавляется новый элемент
        UWrapperRow* Row = *ColArr.Find(col);

        if (Row) {
            bool WidgetNotContains = !Row->Contains(row);
            success = (bool)Row->AddWidget(row, Widget, GridSlot);

            //Новый итем в координатной обёртке защитывается, только если добавление в строку прошло успешно, и если этого итема в ней не было ранее
            if (WidgetNotContains && success)
                NumberOfItemsInTable++;
        }
        else {
            UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the CoordWrapperOfTable class in the AddWidget function: Row is not valid"));
        }
    }
    else {
        //Если строки не существует, она создаётся и в неё добавляется новый элемент
        UPROPERTY()
        UWrapperRow* newRow = NewObject<UWrapperRow>();

        if (newRow) {
            success = newRow->AddWidget(row, Widget, GridSlot);
            if (!success) {
                newRow->ConditionalBeginDestroy();
                newRow->MarkPendingKill();
                return false;
            }

            success = (bool)ColArr.Add(col, newRow);

            if (success)
                NumberOfItemsInTable++;
        }
    }

    //Если новый итем был успешно добавлен, то задаются минимальные и максимальные координаты
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

UAbstractTile* UCoordWrapperOfTable::FindWidget(int row, int col)
{
    if (!ColArr.Contains(col))
        return nullptr;
    UWrapperRow* Row = *ColArr.Find(col);
    if(!Row->Contains(row))
        return nullptr;

    return Row->FindWidget(row);
}

//Поиск слота таблицы, который соотносится с итемом координатной обёртки
UUniformGridSlot* UCoordWrapperOfTable::FindGridSlot(int row, int col)
{
    if (!ColArr.Contains(col))
        return nullptr;
    UWrapperRow* Row = *ColArr.Find(col);
    if (!Row->Contains(row))
        return nullptr;

    return Row->FindGridSlot(row);
}

//Функция, исключающая эллемент из координатной обёртки, но не удаляющая его из мира
bool UCoordWrapperOfTable::RemoveCoord(int row, int col)
{
    if (!ColArr.Contains(col)) {
        return false;
    }
    UWrapperRow* Row = *ColArr.Find(col);
    if (!Row->Contains(row)) {
        return false;
    }

    if (!Row->RemoveIndex(row)) {
        return false;
    }

    if (!Row->HasAnyEllements()) {
        ColArr.Remove(col);
    }

    return true;
}

//Функция, удаляющая эллемент по переданной координате
bool UCoordWrapperOfTable::RemoveWidget(int row, int col)
{
    if (!ColArr.Contains(col)) {
        return false;
    }
    UWrapperRow* Row = *ColArr.Find(col);
    if (!Row->Contains(row)) {
        return false;
    }

    if (!Row->RemoveWidget(row)) {
        return false;
    }

    if (!Row->HasAnyEllements()) {
        ColArr.Remove(col);
    }

    return true;
}

bool UCoordWrapperOfTable::HasAnyEllements()
{
    if(ColArr.Num() == 0)
        return false;
    else {
        TArray<int32> Keys;
        ColArr.GetKeys(Keys);
        for (int32 k : Keys)
        {
            //Проверяется есть ли в существующих столбцах хотя бы одна строка с хотя бы одним элементом
            if ((*ColArr.Find(k))->HasAnyEllements())
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

//Получение самой правой верхней координаты
FGridCoord UCoordWrapperOfTable::getMaxCoord()
{
    return MaxCoord;
}

void UCoordWrapperOfTable::Clear()
{
    TArray<UWrapperRow*> Rows;
    ColArr.GenerateValueArray(Rows);

    for (UWrapperRow* Row : Rows) {
        if (Row && Row->IsValidLowLevel()) {
            Row->Clear();
            Row->ConditionalBeginDestroy();
            Row->MarkPendingKill();
        }
    }
    Rows.Empty();
    ColArr.Empty();

    MinCoord = FGridCoord();
    MaxCoord = FGridCoord();
    NumberOfItemsInTable = 0;
}

void UCoordWrapperOfTable::setMinCoord(FGridCoord minCoord)
{
    this->MinCoord = minCoord;
}

void UCoordWrapperOfTable::setMaxCoord(FGridCoord maxCoord)
{
    this->MaxCoord = maxCoord;
}

//Проверка на то перересекаются ли переданные габариты. Если да, то возвращается true
bool UCoordWrapperOfTable::DoTheDimensionsIntersect(FGridDimensions Dimensions_1, FGridDimensions Dimensions_2)
{
    return Dimensions_1.DoTheDimensionsIntersect(Dimensions_2);
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

//Проверка на то перересекаются ли переданные габариты с текущими. Если да, то возвращается true
bool FGridDimensions::DoTheDimensionsIntersect(FGridDimensions OtherDimensions)
{
    if ((OtherDimensions.Max.Col < Min.Col) || (OtherDimensions.Max.Row < Min.Row) || (OtherDimensions.Min.Col > Max.Col) || (OtherDimensions.Min.Row > Max.Row))
        return false;

    return true;
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