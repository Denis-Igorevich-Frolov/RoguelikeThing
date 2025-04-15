// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/Containers/ExpeditionInteractionObjectContainer.h"

void UExpeditionInteractionObjectContainer::AddData(FString ModuleName, FString CategoryName,
    FString SubCategoryName, FString ObjectId, UExpeditionInteractionObjectData* ExpeditionInteractionObjectData)
{
    FExpeditionInteractionObjectModule& Module = InteractionObjectsModulesArray.FindOrAdd(ModuleName);
    FExpeditionInteractionObjectCategory& Category = Module.InteractionObjectsCategorysArray.FindOrAdd(CategoryName);
    FExpeditionInteractionObjectSubCategory& SubCategory = Category.InteractionObjectsSubCategorysArray.FindOrAdd(SubCategoryName);

    SubCategory.InteractionObjectsDataArray.Add(ObjectId, ExpeditionInteractionObjectData);
}

const UExpeditionInteractionObjectData* const UExpeditionInteractionObjectContainer::FindData(
    FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId)
{
    FExpeditionInteractionObjectModule* Module = InteractionObjectsModulesArray.Find(ModuleName);
    if (Module) {
        FExpeditionInteractionObjectCategory* Category = Module->InteractionObjectsCategorysArray.Find(CategoryName);
        if (Category) {
            FExpeditionInteractionObjectSubCategory* SubCategory = Category->InteractionObjectsSubCategorysArray.Find(SubCategoryName);
            if (SubCategory) {
                UExpeditionInteractionObjectData** ExpeditionInteractionObjectData = SubCategory->InteractionObjectsDataArray.Find(ObjectId);
                if (ExpeditionInteractionObjectData) {
                    return *ExpeditionInteractionObjectData;
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

TMap<FString, UExpeditionInteractionObjectData*> UExpeditionInteractionObjectContainer::GetAllObjects()
{
    TMap<FString, UExpeditionInteractionObjectData*> Result;

    TArray<FExpeditionInteractionObjectModule> Modules;
    InteractionObjectsModulesArray.GenerateValueArray(Modules);

    for (FExpeditionInteractionObjectModule Module : Modules) {
        TArray<FExpeditionInteractionObjectCategory> Categorys;
        Module.InteractionObjectsCategorysArray.GenerateValueArray(Categorys);

        for (FExpeditionInteractionObjectCategory Category : Categorys) {
            TArray<FExpeditionInteractionObjectSubCategory> SubCategorys;
            Category.InteractionObjectsSubCategorysArray.GenerateValueArray(SubCategorys);

            for (FExpeditionInteractionObjectSubCategory SubCategory : SubCategorys) {
                TArray<FString> DataNames;
                SubCategory.InteractionObjectsDataArray.GenerateKeyArray(DataNames);

                for (FString Name : DataNames) {
                    Result.Add(Name, *SubCategory.InteractionObjectsDataArray.Find(Name));
                }
            }
        }
    }

    return Result;
}

TArray<FString> UExpeditionInteractionObjectContainer::GetAllModulesName()
{
    TArray<FString> ModulesName;
    InteractionObjectsModulesArray.GenerateKeyArray(ModulesName);

    return ModulesName;
}

TMap<FString, UExpeditionInteractionObjectData*> UExpeditionInteractionObjectContainer::GetAllObjectsFromeModule(FString ModuleName)
{
    if (!InteractionObjectsModulesArray.Contains(ModuleName)) {
        UE_LOG(LogTemp, Error, TEXT("!InteractionObjectsModulesArray.Contains(ModuleName)"));
        return TMap<FString, UExpeditionInteractionObjectData*>();
    }

    TMap<FString, UExpeditionInteractionObjectData*> Result;

    FExpeditionInteractionObjectModule Module = *InteractionObjectsModulesArray.Find(ModuleName);

    TArray<FExpeditionInteractionObjectCategory> Categorys;
    Module.InteractionObjectsCategorysArray.GenerateValueArray(Categorys);

    for (FExpeditionInteractionObjectCategory Category : Categorys) {
        TArray<FExpeditionInteractionObjectSubCategory> SubCategorys;
        Category.InteractionObjectsSubCategorysArray.GenerateValueArray(SubCategorys);

        for (FExpeditionInteractionObjectSubCategory SubCategory : SubCategorys) {
            TArray<FString> DataNames;
            SubCategory.InteractionObjectsDataArray.GenerateKeyArray(DataNames);

            for (FString Name : DataNames) {
                Result.Add(Name, *SubCategory.InteractionObjectsDataArray.Find(Name));
            }
        }
    }

    return Result;
}
