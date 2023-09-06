// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapEditor.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UMapEditor : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMapSize();
};
