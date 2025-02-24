// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include "RoguelikeThing/GameObjects/InventoryItemData.h"
#include "RoguelikeThing/Lists/InventoryItemsList.h"
#include "InventoryItemsPreparer.generated.h"

/****************************************************************************************************************
 * Данный класс является классом подготовки и загрузки всех всевозможных итемов инвентаря, которые находятся
 * во всех модулях, а также восстановления повреждённых данных связанных с объектами из модуля Default
 ****************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(InventoryItemsPreparer, Log, All);

//Структура подкатегории итемов инвентаря
USTRUCT(BlueprintType)
struct FInventoryItemSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UInventoryItemData*> InventoryItemsDataArray;
};

//Структура категории итемов инвентаря
USTRUCT(BlueprintType)
struct FInventoryItemCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FInventoryItemSubCategory> InventoryItemsSubCategorysArray;
};

//Структура модуля итемов инвентаря
USTRUCT(BlueprintType)
struct FInventoryItemModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FInventoryItemCategory> InventoryItemsCategorysArray;
};

//Контейнер со всеми итемами инвентаря
UCLASS(BlueprintType)
class UInventoryItemsContainer : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TMap<FString, FInventoryItemModule> InventoryItemsModulesArray;

public:
	UFUNCTION(BlueprintCallable)
	void AddData(FString ModuleName, FString CategoryName, FString SubCategoryName,
		FString ObjectId, UInventoryItemData* InventoryItemsData);
	UFUNCTION(BlueprintCallable)
	const UInventoryItemData* const FindData(FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId);
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UInventoryItemsPreparer : public UAbstractPreparer
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
	UInventoryItemData* LoadObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth = 0);

	UInventoryItemsPreparer();

	//Функция получения данных обо всех итемах инвентаря всех модулей
	UFUNCTION(BlueprintCallable)
	void GetAllInventoryItemsData(UPARAM(ref)UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems);
};
