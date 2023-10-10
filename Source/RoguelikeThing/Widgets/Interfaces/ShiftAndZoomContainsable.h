// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShiftAndZoomContainsable.generated.h"

/**
 * 
 */

UINTERFACE(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UShiftAndZoomContainsable : public UInterface
{
    GENERATED_BODY()
};

class IShiftAndZoomContainsable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnContentHasMovedEvent(FVector2D Bias);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnContentHasZoomedEvent(float ZoomMultiplier);
};