// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/Widget.h"
#include "MessageBoxContainsable.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class UMessageBoxContainsable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ROGUELIKETHING_API IMessageBoxContainsable
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnOkWasPressed(UWidget* TextBox);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void OnCancelWasPressed(UWidget* TextBox);
};
