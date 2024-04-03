// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "MapEditor.generated.h"

/************************************************************************
 * ������ ����� �������� ������� ������� ��� ������ ��������� �����.
 * �� ����� ��� ����������� �������������� ���������� � c++ ����.
 ************************************************************************/
UCLASS()
class ROGUELIKETHING_API UMapEditor : public UUserWidget
{
	GENERATED_BODY()
private:
	void ResizeEvent(FViewport* ViewPort, uint32 val);
	//���������� ������ ���� ����������
	FIntPoint OldViewPortSize{0, 0};
	
public:
	UMapEditor(const FObjectInitializer& ObjectInitializer);

	//�����, ���������� �������, ������� ���������� ����� ����� � ��� ����� ���������
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDBCellValue(FCellCoord CellCoord, UMapCell* CallingCell);

	//�����, ��� ������� ������� ����������� ����� ����� ����� ����������, � ����� ������
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateItemAreaContent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnResize(FIntPoint Size);
};
