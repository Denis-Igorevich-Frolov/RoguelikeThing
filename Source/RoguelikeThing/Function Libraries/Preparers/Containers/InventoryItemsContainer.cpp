// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/Containers/InventoryItemsContainer.h"

void UInventoryItemsContainer::AddData(FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId, UInventoryItemData* InventoryItemData)
{
    FInventoryItemModule& Module = InventoryItemsModulesArray.FindOrAdd(ModuleName);
    FInventoryItemCategory& Category = Module.InventoryItemsCategorysArray.FindOrAdd(CategoryName);
    FInventoryItemSubCategory& SubCategory = Category.InventoryItemsSubCategorysArray.FindOrAdd(SubCategoryName);

    SubCategory.InventoryItemsDataArray.Add(ObjectId, InventoryItemData);
}

const UInventoryItemData* const UInventoryItemsContainer::FindData(FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId)
{
    FInventoryItemModule* Module = InventoryItemsModulesArray.Find(ModuleName);
    if (Module) {
        FInventoryItemCategory* Category = Module->InventoryItemsCategorysArray.Find(CategoryName);
        if (Category) {
            FInventoryItemSubCategory* SubCategory = Category->InventoryItemsSubCategorysArray.Find(SubCategoryName);
            if (SubCategory) {
                UInventoryItemData** InventoryItemData = SubCategory->InventoryItemsDataArray.Find(ObjectId);
                if (InventoryItemData) {
                    return *InventoryItemData;
                }
                else {
                    return nullptr;
                }
            }
            else {
                return nullptr;
            }
        }
        else {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
}
