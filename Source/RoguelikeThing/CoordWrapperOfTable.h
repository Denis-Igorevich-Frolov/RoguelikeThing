// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/Widget.h"
#include "CoordWrapperOfTable.generated.h"

/**
 * 
 */
class WrapperRow {
private:
	TMap<int32, UWidget*> Row;
public:
	UWidget* FindWidget(int Key);
	bool AddWidget(int Key, UWidget* Widget);
	bool HasAnyEllements();
	bool Contains(int Key);
};

struct GridCoord
{
private:
	bool isInit = false;

public:
	GridCoord();
	GridCoord(int row, int col);
	bool getIsInit();

	int Col;
	int Row;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UCoordWrapperOfTable : public UObject
{
	GENERATED_BODY()
private:
	TMap<int32, WrapperRow*> Col;
	int NumberOfItemsInTable = 0;

	GridCoord MinCoord;
	GridCoord MaxCoord;

public:
	~UCoordWrapperOfTable();

	UFUNCTION(BlueprintCallable)
	bool AddWidget(int row, int col, UWidget* Widget);
	UFUNCTION(BlueprintCallable)
	UWidget* FindWidget(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool HasAnyEllements();
	UFUNCTION(BlueprintCallable)
	const int getNumberOfItemsInTable();
	UFUNCTION(BlueprintCallable)
	void Clear();

	GridCoord getMinCoord();
	GridCoord getMaxCoord();
};
