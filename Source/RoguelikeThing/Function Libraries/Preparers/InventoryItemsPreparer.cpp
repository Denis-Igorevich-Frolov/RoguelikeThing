// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/InventoryItemsPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <XmlParser/Public/XmlFile.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

DEFINE_LOG_CATEGORY(InventoryItemsPreparer);

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UInventoryItemsPreparer::CheckingDefaultInventoryItems()
{
    CheckingDefaultAbstracts(InventoryItemsList, "Inventory items");
}

//Функция загрузки данных об объекте из его xml файла
UInventoryItemData* UInventoryItemsPreparer::LoadObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth)
{
    auto UploadingData{ [](UInventoryItemsPreparer* Preparer, UInventoryItemData* InventoryItemData, FXmlNode* RootNode, FString FileName,
        UInventoryItemsList* InventoryItemsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth) {
        FXmlNode* MaximumAmountInStackNode = RootNode->FindChildNode("MaximumAmountInStack");
        if (!MaximumAmountInStackNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - Failed to extract MaximumAmountInStack node from file %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        FString StringMaximumAmountInStack = MaximumAmountInStackNode->GetContent();

        if (!StringMaximumAmountInStack.IsNumeric()) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! GALIA U NAS OTMENA %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        int MaximumAmountInStack = FCString::Atoi(*StringMaximumAmountInStack);
        if (MaximumAmountInStack <= 0) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - Name node from file %s is empty"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }
        InventoryItemData->MaximumAmountInStack = MaximumAmountInStack;

        FXmlNode* IsUsableNode = RootNode->FindChildNode("IsUsable");
        if (!IsUsableNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - Failed to extract IsUsable node from file %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        FString StringIsUsable = IsUsableNode->GetContent();
        StringIsUsable = StringIsUsable.ToLower();

        if (!(StringIsUsable == "true" || StringIsUsable == "false")) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! GALIA U NAS OTMENA 2 %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        bool IsUsable = (StringIsUsable == "true" ? true : false);
        InventoryItemData->IsUsable = IsUsable;

        FXmlNode* AllowedForUseOnExpeditionInteractionObjectsNode = RootNode->FindChildNode("AllowedForUseOnExpeditionInteractionObjects");
        if (!AllowedForUseOnExpeditionInteractionObjectsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - Failed to extract AllowedForUseOnInventoryItemss node from file %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        FString StringAllowedForUseOnInventoryItemss = AllowedForUseOnExpeditionInteractionObjectsNode->GetContent();
        StringAllowedForUseOnInventoryItemss = StringAllowedForUseOnInventoryItemss.ToLower();

        if (!(StringAllowedForUseOnInventoryItemss == "true" || StringAllowedForUseOnInventoryItemss == "false")) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! GALIA U NAS OTMENA 3 %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        bool AllowedForUseOnInventoryItemss = (StringAllowedForUseOnInventoryItemss == "true" ? true : false);
        InventoryItemData->AllowedForUseOnInventoryItemss = AllowedForUseOnInventoryItemss;

        FXmlNode* UseEventsNode = RootNode->FindChildNode("UseEvents");
        if (!UseEventsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - Failed to extract UseEvents node from file %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        TArray<FXmlNode*> UseEvents = UseEventsNode->GetChildrenNodes();
        for (FXmlNode* EventNode : UseEvents) {
            if (!EventNode) {
                UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - EventNode from file %s is not valid"), *XMLFilePath);

                if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                    if (InventoryItemData->IsRooted())
                        InventoryItemData->RemoveFromRoot();

                    InventoryItemData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                        UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadInventoryItemFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                UInventoryItemData* Plug = nullptr;
                return Plug;
            }

            FUseEvent UseEvent;
            InventoryItemData->UseEvents.Add(EventNode->GetTag(), UseEvent);
        }

        return InventoryItemData;
    } };
    return LoadDataFromXML<UInventoryItemData, UInventoryItemsList, UInventoryItemsPreparer>(
        this, ModuleName, "Inventory items", XMLFilePath, FileManager, InventoryItemsList, UploadingData, RecursionDepth);
}

UInventoryItemsPreparer::UInventoryItemsPreparer()
{
    InventoryItemsList = CreateDefaultSubobject<UInventoryItemsList>("InventoryItemsList");
}

//Функция получения данных обо всех объектах взаимодействия всех модулей
void UInventoryItemsPreparer::GetAllInventoryItemsData(UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems)
{
    GetAllData<UInventoryItemsContainer, UInventoryItemData, UInventoryItemsPreparer>(
        InventoryItemsContainer, ModsDirWithInventoryItems, InventoryItemsList, "Inventory items", this);
}

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
