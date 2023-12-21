// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapCell.generated.h"

/*****************************************************************
 * Данный класс является базовым классом для виджета ячейки. Он
 * нужен для правильного взаимодействия блюпринтов и c++ кода.
 *****************************************************************/
UCLASS()
class ROGUELIKETHING_API UMapCell : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();
};
