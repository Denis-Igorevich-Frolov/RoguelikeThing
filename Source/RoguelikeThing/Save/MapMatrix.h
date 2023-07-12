// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SQLiteDatabase.h"
#include "MapMatrix.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapMatrix : public UObject
{
	GENERATED_BODY()
	
public:
	FSQLiteDatabase* db = new FSQLiteDatabase();

	UFUNCTION(BlueprintCallable)
	void Test();
};
