// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <RoguelikeThing/CoordWrapperOfTable.h>
#include <Components/UniformGridPanel.h>
#include "MapTile.generated.h"

/*****************************************************************
 * ������ ����� �������� ������� ������� ��� ������� �����. ��
 * ����� ��� ����������� �������������� ���������� � c++ ����.
 *****************************************************************/
UCLASS()
class ROGUELIKETHING_API UMapTile : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMapTile(const FObjectInitializer& Object);

	//������������ ������, �������� � ���� ������ ����� �����
	UCoordWrapperOfTable* CellsCoordWrapper;

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();
};
