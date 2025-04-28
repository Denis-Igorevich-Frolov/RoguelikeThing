// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"
#include "RoguelikeThing/Function Libraries/Preparers/Containers/ExpeditionInteractionObjectContainer.h"
#include "RoguelikeThing/Interfaces/TexturesLoadableInterface.h"
#include "RoguelikeThing/Interfaces/SoundsLoadableInterface.h"
#include "ExpeditionInteractionObjectPreparer.generated.h"

/*************************************************************************************************************************
 * Данный класс является классом подготовки и загрузки всех всевозможных объектов взаимодействия экспедиции, которые
 * находятся во всех модулях, а также восстановления повреждённых данных связанных с объектами из модуля Default
 *************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(ExpeditionInteractionObjectPreparer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectPreparer : public UAbstractPreparer, public ITexturesLoadableInterface, public ISoundsLoadableInterface
{
	GENERATED_BODY()

private:
	//Список исходников всех объектов из модуля Default для возможного их восстановления
	UPROPERTY()
	UExpeditionInteractionObjectsList* ExpeditionInteractionObjectsList;

	//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
	void CheckingDefaultExpeditionInteractionObjects();
	
public:
	UExpeditionInteractionObjectPreparer();

	//Функция загрузки данных об объекте из его xml файла
	UExpeditionInteractionObjectData* LoadObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth = 0);

	//Функция получения данных обо всех объектах взаимодействия всех модулей
	UFUNCTION(BlueprintCallable)
	void GetAllObjectsData(UPARAM(ref)UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, TArray<FString> ModsDirWithInteractionObjects);
};
