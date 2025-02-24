// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RoguelikeThing/SaveGame/BinArrayWrapper.h"
#include "DataSaver.generated.h"

/***************************************************************************************
 * Данный класс является классом, создающим файл сохранения .sav для любого отдельно
 * взятого модуля данных из папки Data, а также загружающим данные из этого файла
 ***************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(DataSaver, Log, All);

UCLASS()
class ROGUELIKETHING_API UDataSaver : public USaveGame
{
	GENERATED_BODY()
	
protected:

	/* Коллекция бинарных данных объектов модуля, ключом которой
	 * является путь до соответствующего xml файла, а значением -
	 * сериализованные данные полученные из этого xml файла */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FBinArrayWrapper> BinData;

	/* Коллекция хешей xml файлов объектов модуля, ключом которой является
	 * путь до соответствующего xml файла, а значением - его хеш */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FString> XMLFilesHash;

	//Путь до sav файла данного модуля
	UPROPERTY(VisibleAnywhere, SaveGame)
	FString SavFilePath;

public:

	/* Функция, добавляющая данные о объекте в массив всех объектов соответствующего
	 * модуля для последующего сохранения. Также сохраняется хеш исходного xml файла */
	template<typename Data> 
	void AddDataToBinArray(Data* ObjectData, FString SavedLocalFilePath, FString FullFilePath);
	/* Получение коллекции всех объектов загружаемого модуля. Ключом коллекции является
	 * путь до исходного xml файла, а значением - непосредственно данные этого объекта.
	 * Из-за создания экземпляров UObject эту функцию безопасно запускать только в основном потоке */
	template<typename Data>
	TMap<FString, Data*> GetData();
	/* Полностью сформированный массив объектов относящихся к сохраняемому модулю,
	 * записывается в sav файл, который будет находится по переданному пути */
	void SaveBinArray(FString FilePath);
	/* Загрузка массива объектов модуля, который был сохранён в слот по переданному пути.
	 * Чтобы затем извлечь этот массив, следует воспользоваться функцией GetData */
	void LoadBinArray(FString FilePath);
	//Проверка изменения хешей xml фалов, связанных с текущим загруженным модулем. true - изменений нет, false - есть
	bool CheckHashChange();
	//Получение количества объектов, а соответственно и xml файлов, на основании которых была сформирована информация о всех объктах модуля
	int GetBinArraySize();
	//Полная очистка всех массивов данного модуля
	void ClearArray();

	const FString GetSavFilePath();
};
