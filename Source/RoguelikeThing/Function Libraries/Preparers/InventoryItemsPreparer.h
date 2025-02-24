// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include "RoguelikeThing/GameObjects/InventoryItemData.h"
#include "RoguelikeThing/Lists/InventoryItemsList.h"
#include "InventoryItemsPreparer.generated.h"

/****************************************************************************************************************
 * ������ ����� �������� ������� ���������� � �������� ���� ������������ ������ ���������, ������� ���������
 * �� ���� �������, � ����� �������������� ����������� ������ ��������� � ��������� �� ������ Default
 ****************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(InventoryItemsPreparer, Log, All);

//��������� ������������ ������ ���������
USTRUCT(BlueprintType)
struct FInventoryItemSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UInventoryItemData*> InventoryItemsDataArray;
};

//��������� ��������� ������ ���������
USTRUCT(BlueprintType)
struct FInventoryItemCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FInventoryItemSubCategory> InventoryItemsSubCategorysArray;
};

//��������� ������ ������ ���������
USTRUCT(BlueprintType)
struct FInventoryItemModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FInventoryItemCategory> InventoryItemsCategorysArray;
};

//��������� �� ����� ������� ���������
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
	//������ ���������� ���� �������� �� ������ Default ��� ���������� �� ��������������
	UPROPERTY()
	UInventoryItemsList* InventoryItemsList;

	//�������� ������������� �������� �� ������ Default. ��� ����������� �� ����������, ��� ����������������� �� ��������� ������
	void CheckingDefaultInventoryItems();

public:
	//������� �������� ������ �� ������� �� ��� xml �����
	UInventoryItemData* LoadObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth = 0);

	UInventoryItemsPreparer();

	//������� ��������� ������ ��� ���� ������ ��������� ���� �������
	UFUNCTION(BlueprintCallable)
	void GetAllInventoryItemsData(UPARAM(ref)UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems);
};
