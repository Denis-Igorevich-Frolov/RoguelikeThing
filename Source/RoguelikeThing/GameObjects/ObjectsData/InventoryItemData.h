// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/GameObjects/ObjectsData/AbstractData.h"
#include "InventoryItemData.generated.h"

/*****************************************************************************************************************
 * Данный класс является классом полного описания всех свойств и ивентов одного отдельно взятого итема инвентаря
 *****************************************************************************************************************/

 //Структура эвентов при нажатии на итем.	!!!!!На данный момент находящаяся в состоянии зачаточном и существующая для подготовки методов, сопряжённых с её будущим функционалом
USTRUCT(BlueprintType)
struct FUseEvent
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UInventoryItemData : public UAbstractData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString ModuleLocalPath;

	//Максимальное количество итемов данного типа, которое будет умещаться в 1 ячейку инвентаря
	UPROPERTY(BlueprintReadWrite, SaveGame)
	int MaximumAmountInStack;

	/* Переменная, показывающая можно ли этот итем прожимать на объекты
	 * взаимодействия экспедиции. Ивенты при этом будут вызываться не те, что
	 * перечислены в UseEvents, а те, которые определит объект экспедиции */
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool AllowedForUseOnExpeditionInteractionObjects;

	//Ивенты, вызываемые при нажатии по итему. Если массив пуст, то предмет неюзабельный
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FUseEvent> UseEvents;
};
