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

private:
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("/Save/Map.db");

	FSQLiteDatabase* mapDataBase = new FSQLiteDatabase();
	FSQLitePreparedStatement* LoadStatement = new FSQLitePreparedStatement();

public:
	UMapMatrix();
	~UMapMatrix();

	UFUNCTION(BlueprintCallable)
	void Test();
};
