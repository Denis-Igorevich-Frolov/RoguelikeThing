// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/Map/MapMatrix.h"
#include <Components/UniformGridPanel.h>
#include "FillingMapWithCells.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	ULoadingWidget* LoadingWidget;
	
public:
	UFUNCTION(BlueprintCallable)
	void FillMapEditorWithCells(FMapDimensions MapDimensions, int32 TableLength, UUniformGridPanel* GridPanel, UUserWidget* SampleWidget);

	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);
};
