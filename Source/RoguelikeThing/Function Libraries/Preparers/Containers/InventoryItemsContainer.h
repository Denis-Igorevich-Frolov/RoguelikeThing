// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoguelikeThing/GameObjects/ObjectsData/InventoryItemData.h"
#include "InventoryItemsContainer.generated.h"

/**
 * 
 */

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
