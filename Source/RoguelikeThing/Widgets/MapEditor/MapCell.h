// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"
#include "MapCell.generated.h"

/*****************************************************************
 * ������ ����� �������� ������� ������� ��� ������� ������. ��
 * ����� ��� ����������� �������������� ���������� � c++ ����.
 *****************************************************************/
UCLASS()
class ROGUELIKETHING_API UMapCell : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMapEditorBrushType* BrushType;
};
