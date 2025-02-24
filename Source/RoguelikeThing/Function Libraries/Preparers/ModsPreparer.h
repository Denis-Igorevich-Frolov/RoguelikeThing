// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ModsPreparer.generated.h"

/*****************************************************************************************
 * Данный класс служит для получения списков директорий всех модулей модов, включающих
 * в себя определённые дополнения или переопределения конкретных групп объектов игры.
 *****************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(ModsPreparer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UModsPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//Функция, возвращающая список директорий модулей модов по переданному названию группы объектов, которую изменяет мод
	TArray<FString> GetArrayOfModDirectories(FString CategoryName);
	
public:
	//Функция, возвращающая список директорий модулей модов, которые изменяют список интерактивных объектов экспедиции
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetArrayOfModDirectoriesHavingExpeditionInteractionObjects();
	//Функция, возвращающая список директорий модулей модов, которые изменяют список итемов инвентаря
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetArrayOfModDirectoriesHavingInventoryItems();
};
