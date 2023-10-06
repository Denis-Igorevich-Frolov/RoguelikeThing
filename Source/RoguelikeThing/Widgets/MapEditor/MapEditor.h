// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Widgets/Interfaces/ShiftAndZoomContainsable.h"
#include "MapEditor.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UMapEditor : public UUserWidget, public IShiftAndZoomContainsable
{
	GENERATED_BODY()
private:
	void ResizeEvent(FViewport* ViewPort, uint32 val);
	FIntPoint OldViewPortSize{0, 0};
	
public:
	UMapEditor(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMapSize();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateItemAreaSize(FIntPoint Size);
};
