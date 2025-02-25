// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/ObjectsData/AbstractData.h"
#include "RoguelikeThing/Lists/AbstractList.h"
#include <XmlParser/Public/XmlFile.h>
#include "AbstractPreparer.generated.h"

/******************************************************************************************************************
 * Данный класс является базовым для классов подготовки и загрузки всех всевозможных объектов конкретного типа
 ******************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(AbstractPreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfTheDownloadDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:

	//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
	void CheckingDefaultAbstractObjects(UAbstractList* ObjectsList, FString ModuleRoot);
	//Функция загрузки данных об объекте из его xml файла
	template<typename DataType, typename ListType, typename PreparerType>
	DataType* LoadDataFromXML(PreparerType* Preparer, FString ModuleName, FString ModuleRoot, FString XMLFilePath, IPlatformFile& FileManager,
		ListType* ObjectsList, DataType*(*UploadingData)(PreparerType* Preparer, DataType* Data, FXmlNode* RootNode, FString FileName,
			ListType* ObjectsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, FXmlFile* XmlFile, int RecursionDepth), int RecursionDepth = 0);

	//Функция восстановления файла из моодуля Default по его имени (без расширения)
	bool RestoringDefaultFileByName(FString Name, FString ModuleRoot, UAbstractList* ObjectsList);

	template<typename DataType>
	void EmergencyResetOfPointers(DataType* Data, FXmlFile* XmlFile);

public:

	//Функция получения данных обо всех объектах конкретного типа всех модулей
	template<typename Container, typename DataType, typename PreparerType>
	void GetAllData(UPARAM(ref)Container* DataContainer, TArray<FString> ModsDirWithAbstract, UAbstractList* ObjectsList, FString ModuleRoot, PreparerType* Preparer);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfTheDownloadDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FLoadingCompleteDelegate LoadingComplete;
};
