// Fill out your copyright notice in the Description page of Project Settings.

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
	void AddWidget(int Key, UWidget* Widget);
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UCoordWrapperOfTable : public UObject
{
	GENERATED_BODY()
private:
	TMap<int32, WrapperRow*> Col;
public:
	~UCoordWrapperOfTable();

	UFUNCTION(BlueprintCallable)
	void AddWidget(int col, int row, UWidget* Widget);
	UFUNCTION(BlueprintCallable)
	UWidget* FindWidget(int col, int row);
};
