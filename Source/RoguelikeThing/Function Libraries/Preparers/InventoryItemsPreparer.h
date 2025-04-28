// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include "RoguelikeThing/Function Libraries/Preparers/Containers/InventoryItemsContainer.h"
#include "RoguelikeThing/Lists/InventoryItemsList.h"
#include "RoguelikeThing/Interfaces/TexturesLoadableInterface.h"
#include "InventoryItemsPreparer.generated.h"

/****************************************************************************************************************
 * ������ ����� �������� ������� ���������� � �������� ���� ������������ ������ ���������, ������� ���������
 * �� ���� �������, � ����� �������������� ����������� ������ ��������� � ��������� �� ������ Default
 ****************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(InventoryItemsPreparer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UInventoryItemsPreparer : public UAbstractPreparer, public ITexturesLoadableInterface
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
	UInventoryItemData* LoadObjectFromXML(FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth = 0);

	UInventoryItemsPreparer();

	//������� ��������� ������ ��� ���� ������ ��������� ���� �������
	UFUNCTION(BlueprintCallable)
	void GetAllInventoryItemsData(UPARAM(ref)UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems);
};
