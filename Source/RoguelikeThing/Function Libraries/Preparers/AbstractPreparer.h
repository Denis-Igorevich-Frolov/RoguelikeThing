// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/AbstractData.h"
#include "RoguelikeThing/Lists/AbstractList.h"
#include <XmlParser/Public/XmlFile.h>
#include "AbstractPreparer.generated.h"

/*************************************************************************************************************************
 * ������ ����� �������� ������� ���������� � �������� ���� ������������ �������� �������������� ����������, �������
 * ��������� �� ���� �������, � ����� �������������� ����������� ������ ��������� � ��������� �� ������ Default
 *************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(AbstractPreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfTheDownloadDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLoadingCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	//�������� ������������� �������� �� ������ Default. ��� ����������� �� ����������, ��� ����������������� �� ��������� ������
	void CheckingDefaultAbstracts(UAbstractList* ObjectsList);
	//������� �������� ������ �� ������� �� ��� xml �����
	template<typename DataType, typename ListType, typename PreparerType>
	DataType* LoadDataFromXML(PreparerType* Preparer, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, ListType* ObjectsList,
		DataType*(*UploadingData)(PreparerType* Preparer, DataType* Data, FXmlNode* RootNode, FString FileName, ListType* ObjectsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth), int RecursionDepth = 0);

	//������� �������������� ����� �� ������� Default �� ��� ����� (��� ����������)
	UFUNCTION(BlueprintCallable)
	bool RestoringDefaultFileByName(FString Name, UAbstractList* ObjectsList);

	//������� ��������� ������ ��� ���� �������� �������������� ���� �������
	template<typename Container, typename DataType, typename PreparerType>
	void GetAllData(UPARAM(ref)Container* DataContainer, TArray<FString> ModsDirWithAbstract, UAbstractList* ObjectsList, PreparerType* Preparer);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfTheDownloadDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FLoadingCompleteDelegate LoadingComplete;
};
