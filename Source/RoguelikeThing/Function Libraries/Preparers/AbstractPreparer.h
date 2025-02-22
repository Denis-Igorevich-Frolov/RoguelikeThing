// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/AbstractData.h"
#include "RoguelikeThing/Lists/AbstractList.h"
#include <XmlParser/Public/XmlFile.h>
#include "AbstractPreparer.generated.h"

/*************************************************************************************************************************
 * Данный класс является классом подготовки и загрузки всех всевозможных объектов взаимодействия экспедиции, которые
 * находятся во всех модулях, а также восстановления повреждённых данных связанных с объектами из модуля Default
 *************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(AbstractPreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfTheDownloadDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
	void CheckingDefaultAbstracts(UAbstractList* ObjectsList);
	//Функция загрузки данных об объекте из его xml файла
	template<typename DataType, typename ListType, typename PreparerType>
	DataType* LoadDataFromXML(PreparerType* Preparer, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, ListType* ObjectsList,
		DataType*(*UploadingData)(PreparerType* Preparer, DataType* Data, FXmlNode* RootNode, FString FileName, ListType* ObjectsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth), int RecursionDepth = 0);

	//Функция восстановления файла из моодуля Default по его имени (без расширения)
	UFUNCTION(BlueprintCallable)
	bool RestoringDefaultFileByName(FString Name, UAbstractList* ObjectsList);

	//Функция получения данных обо всех объектах взаимодействия всех модулей
	template<typename Container, typename DataType, typename PreparerType>
	void GetAllData(UPARAM(ref)Container* DataContainer, TArray<FString> ModsDirWithAbstract, UAbstractList* ObjectsList, PreparerType* Preparer);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfTheDownloadDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FLoadingCompleteDelegate LoadingComplete;
};
