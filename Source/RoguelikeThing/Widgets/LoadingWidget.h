// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

/********************************************************
 * Данный класс является базовым бдя виджета загрузки
 ********************************************************/
UCLASS()
class ROGUELIKETHING_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void LoadingComplete(bool success);
};
