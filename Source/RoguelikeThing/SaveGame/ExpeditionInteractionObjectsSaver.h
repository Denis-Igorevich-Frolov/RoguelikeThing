// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h>
#include "ExpeditionInteractionObjectsSaver.generated.h"

/**********************************************************************
 * Данный класс является классом, создающим файл сохранения .sav
 * для отдельно взятого модуля объектов взаимодействия экспедиции.
 **********************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(ExpeditionInteractionObjectsSaver, Log, All);

//Структура, позволяющая обойти ограничение движка на создание массива массивов
USTRUCT()
struct FBinArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TArray<uint8> BinArray;
};

UCLASS()
class ROGUELIKETHING_API UExpeditionInteractionObjectsSaver : public USaveGame
{
	GENERATED_BODY()
	
protected:

	/* Коллекция бинарных данных объектов взаимодействия экспедиции, ключом
	 * которой является путь до соответствующего xml файла, а значением -
	 * сериализованные данные полученные из этого xml файла */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FBinArrayWrapper> BinExpeditionInteractionObjectsData;

	/* Коллекция хешей xml файлов объектов взаимодействия экспедиции, ключом которой
	 * является путь до соответствующего xml файла, а значением - его хеш */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FString> XMLFilesHash;

public:

	/* Функция, добавляющая данные о предмете взаимодействия экспедиции в массив всех объектов
	 * соответствующего модуля для последующего сохранения. Также сохраняется хеш исходного xml файла */
	void AddExpeditionInteractionObjectDataToBinArray(UExpeditionInteractionObjectData* ExpeditionInteractionObjectData, FString FilePath);
	/* Получение коллекции всех объектов взаимодействия экспедиции загруженного модуля. Ключом коллекции
	 * является путь до исходного xml файла, а значением - непосредственно данные по этому объекту */
	TMap<FString, UExpeditionInteractionObjectData*> GetExpeditionInteractionObjectsData();
	/* Полностью сформированный массив объектов взаимодействия экспедиции, относящихся к
	 * одному модулю, сохраняется в sav файл, который будет находится по переданному пути */
	void SaveBinArray(FString FilePath);
	/* Загрузка массива объектов взаимодействия экспедиции, который был сохранён в слот по переданному пути.
	 * Чтобы затем извлечь этот массив, следует воспользоваться функцией GetExpeditionInteractionObjectsData */
	void LoadBinArray(FString FilePath);
	//Проверка изменения хешей xml фалов, связанных с текущим загруженным модулем. true - изменений нет, false - есть
	bool CheckHashChange();
	//Получение количества предметов, а соответственно и xml файлов, на основании которых была сформирована информация о всех предметах модуля
	int GetBinArraySize();
	//Полная очистка всех массивов данного модуля
	void ClearArray();
};
