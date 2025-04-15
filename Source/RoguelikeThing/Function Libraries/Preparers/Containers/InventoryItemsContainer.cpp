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

TMap<FString, UInventoryItemData*> UInventoryItemsContainer::GetAllObjects()
{
    TMap<FString, UInventoryItemData*> Result;

    TArray<FInventoryItemModule> Modules;
    InventoryItemsModulesArray.GenerateValueArray(Modules);

    for (FInventoryItemModule Module : Modules) {
        TArray<FInventoryItemCategory> Categorys;
        Module.InventoryItemsCategorysArray.GenerateValueArray(Categorys);

        for (FInventoryItemCategory Category : Categorys) {
            TArray<FInventoryItemSubCategory> SubCategorys;
            Category.InventoryItemsSubCategorysArray.GenerateValueArray(SubCategorys);

            for (FInventoryItemSubCategory SubCategory : SubCategorys) {
                TArray<FString> DataNames;
                SubCategory.InventoryItemsDataArray.GenerateKeyArray(DataNames);

                for (FString Name : DataNames) {
                    Result.Add(Name, *SubCategory.InventoryItemsDataArray.Find(Name));
                }
            }
        }
    }

    return Result;
}

TArray<FString> UInventoryItemsContainer::GetAllModulesName()
{
    TArray<FString> ModulesName;
    InventoryItemsModulesArray.GenerateKeyArray(ModulesName);

    return ModulesName;
}

TMap<FString, UInventoryItemData*> UInventoryItemsContainer::GetAllObjectsFromeModule(FString ModuleName)
{
    if (!InventoryItemsModulesArray.Contains(ModuleName)) {
        UE_LOG(LogTemp, Error, TEXT("!InventoryItemsModulesArray.Contains(ModuleName)"));
        return TMap<FString, UInventoryItemData*>();
    }

    TMap<FString, UInventoryItemData*> Result;

    FInventoryItemModule Module = *InventoryItemsModulesArray.Find(ModuleName);

    TArray<FInventoryItemCategory> Categorys;
    Module.InventoryItemsCategorysArray.GenerateValueArray(Categorys);

    for (FInventoryItemCategory Category : Categorys) {
        TArray<FInventoryItemSubCategory> SubCategorys;
        Category.InventoryItemsSubCategorysArray.GenerateValueArray(SubCategorys);

        for (FInventoryItemSubCategory SubCategory : SubCategorys) {
            TArray<FString> DataNames;
            SubCategory.InventoryItemsDataArray.GenerateKeyArray(DataNames);

            for (FString Name : DataNames) {
                Result.Add(Name, *SubCategory.InventoryItemsDataArray.Find(Name));
            }
        }
    }

    return Result;
}
