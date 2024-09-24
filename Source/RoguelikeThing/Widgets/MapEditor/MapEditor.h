// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Interfaces/ContainingTileTableInterface.h"
#include "MapEditor.generated.h"

/************************************************************************
 * Данный класс является базовым классом для экрана редактора карты.
 * Он нужен для правильного взаимодействия блюпринтов и c++ кода.
 ************************************************************************/
UCLASS()
class ROGUELIKETHING_API UMapEditor : public UUserWidget, public IContainingTileTableInterface
{
	GENERATED_BODY()
private:
	void ResizeEvent(FViewport* ViewPort, uint32 val);
	//Предыдущий размер окна приложения
	FIntPoint OldViewPortSize{0, 0};
	
public:
	UMapEditor(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnResize(FIntPoint Size);
};
