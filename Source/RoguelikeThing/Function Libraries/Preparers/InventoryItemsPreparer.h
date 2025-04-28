// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include "RoguelikeThing/Function Libraries/Preparers/Containers/InventoryItemsContainer.h"
#include "RoguelikeThing/Lists/InventoryItemsList.h"
#include "RoguelikeThing/Interfaces/TexturesLoadableInterface.h"
#include "InventoryItemsPreparer.generated.h"

/****************************************************************************************************************
 * Данный класс является классом подготовки и загрузки всех всевозможных итемов инвентаря, которые находятся
 * во всех модулях, а также восстановления повреждённых данных связанных с объектами из модуля Default
 ****************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(InventoryItemsPreparer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UInventoryItemsPreparer : public UAbstractPreparer, public ITexturesLoadableInterface
{
	GENERATED_BODY()

private:
	//Список исходников всех объектов из модуля Default для возможного их восстановления
	UPROPERTY()
	UInventoryItemsList* InventoryItemsList;

	//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
	void CheckingDefaultInventoryItems();

public:
	//Функция загрузки данных об объекте из его xml файла
	UInventoryItemData* LoadObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth = 0);

	UInventoryItemsPreparer();

	//Функция получения данных обо всех итемах инвентаря всех модулей
	UFUNCTION(BlueprintCallable)
	void GetAllInventoryItemsData(UPARAM(ref)UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems);
};
