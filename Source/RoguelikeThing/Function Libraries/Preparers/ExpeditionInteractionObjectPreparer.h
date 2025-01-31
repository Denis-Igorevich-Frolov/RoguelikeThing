// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h"
#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"
#include "ExpeditionInteractionObjectPreparer.generated.h"

/*************************************************************************************************************************
 * Данный класс является классом подготовки и загрузки всех всевозможных объектов взаимодействия экспедиции, которые
 * находятся во всех модулях, а также восстановления повреждённых данных связанных с объектами из модуля Default
 *************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(ExpeditionInteractionObjectPreparer, Log, All);

//Структура подкатегории объектов взаимодействия
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UExpeditionInteractionObjectData*> InteractionObjectsDataArray;
};

//Структура категории объектов взаимодействия
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FExpeditionInteractionObjectSubCategory> InteractionObjectsSubCategorysArray;
};

//Структура модуля объектов взаимодействия
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FExpeditionInteractionObjectCategory> InteractionObjectsCategorysArray;
};

//Контейнер со всеми объектами взаимодействия
UCLASS(BlueprintType)
class UExpeditionInteractionObjectContainer : public UObject
{
	GENERATED_BODY()

private:

	UPROPERTY()
	TMap<FString, FExpeditionInteractionObjectModule> InteractionObjectsModulesArray;

public:
	FString InteractionObjectsModulesArray2;

	UFUNCTION(BlueprintCallable)
	void AddExpeditionInteractionObjectData(FString ModuleName, FString CategoryName, FString SubCategoryName,
		FString ObjectId, UExpeditionInteractionObjectData* ExpeditionInteractionObjectData);
	UFUNCTION(BlueprintCallable)
	const UExpeditionInteractionObjectData* const FindExpeditionInteractionObjectData(FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId);
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:

	//Список исходников всех объектов из модуля Default для возможного их восстановления
	ExpeditionInteractionObjectsList expeditionInteractionObjectsList;

	//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
	void CheckingDefaultExpeditionInteractionObjects();
	//Функция загрузки данных об объекте из его xml файла
	UExpeditionInteractionObjectData* LoadExpeditionInteractionObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth = 0);
	
public:
	//Функция восстановления файла из моодуля Default по его имени (без расширения)
	UFUNCTION(BlueprintCallable)
	bool RestoringDefaultFileByName(FString Name);

	//Функция получения данных обо всех объектах взаимодействия всех модулей
	UFUNCTION(BlueprintCallable)
	void GetAllExpeditionInteractionObjectsData(UPARAM(ref)UExpeditionInteractionObjectContainer*& ExpeditionInteractionObjectContainer);
};
