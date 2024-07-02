// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <RoguelikeThing/CoordWrapperOfTable.h>
#include <Components/UniformGridPanel.h>
#include <RoguelikeThing/MyGameInstance.h>
#include "MapTile.generated.h"

/*****************************************************************
 * ������ ����� �������� ������� ������� ��� ������� �����. ��
 * ����� ��� ����������� �������������� ���������� � c++ ����.
 *****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(Map_Tile, Log, All);

UCLASS()
class ROGUELIKETHING_API UMapTile : public UUserWidget
{
	GENERATED_BODY()

private:
	//�������� �������� ������ ��� ���������� ���������� ����
	UPROPERTY()
	UMyGameInstance* GameInstance;
	
public:
	UMapTile(const FObjectInitializer& Object);

	//������������ ������, �������� � ���� ������ ����� �����
	UCoordWrapperOfTable* CellsCoordWrapper;

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();

	UFUNCTION(BlueprintCallable)
	bool FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor);
};
