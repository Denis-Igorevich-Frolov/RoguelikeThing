// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/UniformGridPanel.h>
#include "MapTile.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UMapTile : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();
};
