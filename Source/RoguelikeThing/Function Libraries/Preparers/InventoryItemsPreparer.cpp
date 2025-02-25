// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/InventoryItemsPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <XmlParser/Public/XmlFile.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

DEFINE_LOG_CATEGORY(InventoryItemsPreparer);

//�������� ������������� �������� �� ������ Default. ��� ����������� �� ����������, ��� ����������������� �� ��������� ������
void UInventoryItemsPreparer::CheckingDefaultInventoryItems()
{
    CheckingDefaultAbstractObjects(InventoryItemsList, "Inventory items");
}

//������� �������� ������ �� ������� �� ��� xml �����
UInventoryItemData* UInventoryItemsPreparer::LoadObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth)
{
    auto UploadingData{ [](UInventoryItemsPreparer* Preparer, UInventoryItemData* InventoryItemData, FXmlNode* RootNode, FString FileName,
        UInventoryItemsList* InventoryItemsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, FXmlFile* XmlFile, int RecursionDepth) {
        FXmlNode* MaximumAmountInStackNode = RootNode->FindChildNode("MaximumAmountInStack");
        if (!MaximumAmountInStackNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract MaximumAmountInStack node from file %s"), *XMLFilePath);

            if (InventoryItemData && InventoryItemData->IsValidLowLevel()) {
                if (InventoryItemData->IsRooted())
                    InventoryItemData->RemoveFromRoot();

                InventoryItemData->MarkPendingKill();
            }

            //���� ���� ��� �� ������ Default, �� ������������ ������� ��� �������������� �� ��������� ������
            if (ModuleName == "Default") {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    /* � ����� ��� ��������������, ���������� ����������� ��� �� ������� � ����� ��� ��� ����������
                     * ��������� ��� ��������������� ����. ���������� Preparer - ��� ��������� �� ������� ���������,
                     * ��� ��� �������� ����� �� ������� �������� ������ � ������� �������� ��������� ���������� */
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    //���� �� ���� ������������ �� ������, ������������ ��������� ���������� ���������
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            //������ ������ � ���� ������������ ������, ��� ��� ����� ������� nullptr ���������� ����� �������
            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        FString StringMaximumAmountInStack = MaximumAmountInStackNode->GetContent();

        if (!StringMaximumAmountInStack.IsNumeric()) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Text in MaximumAmountInStack node from file %s is not numeric"), *XMLFilePath);

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
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        int MaximumAmountInStack = FCString::Atoi(*StringMaximumAmountInStack);
        if (MaximumAmountInStack <= 0) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Number in MaximumAmountInStack node from file %s is less than one"), *XMLFilePath);

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
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }
        InventoryItemData->MaximumAmountInStack = MaximumAmountInStack;

        FXmlNode* AllowedForUseOnExpeditionInteractionObjectsNode = RootNode->FindChildNode("AllowedForUseOnExpeditionInteractionObjects");
        if (!AllowedForUseOnExpeditionInteractionObjectsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract AllowedForUseOnExpeditionInteractionObjects node from file %s"), *XMLFilePath);

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
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        FString StringAllowedForUseOnExpeditionInteractionObjects = AllowedForUseOnExpeditionInteractionObjectsNode->GetContent();
        //����� ����� ���� ���������� � ������� ��������, ��� ��� ��������� ������ ������� ���������� � xml ����� ��� ������
        StringAllowedForUseOnExpeditionInteractionObjects = StringAllowedForUseOnExpeditionInteractionObjects.ToLower();

        //���� ����� ���� �� ����� true ��� false, �� ��� ������. ������ ���� ��������� ������� ���������� �� ������� �������� � �� �����������.
        if (!(StringAllowedForUseOnExpeditionInteractionObjects == "true" || StringAllowedForUseOnExpeditionInteractionObjects == "false")) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Text of the AllowedForUseOnExpeditionInteractionObjects node from file %s is not \"true\" or \"false\". You can write bool variables in any case, but you cannot denote them with numbers or anything else."), *XMLFilePath);

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
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        bool AllowedForUseOnExpeditionInteractionObjects = (StringAllowedForUseOnExpeditionInteractionObjects == "true" ? true : false);
        InventoryItemData->AllowedForUseOnExpeditionInteractionObjects = AllowedForUseOnExpeditionInteractionObjects;

        FXmlNode* UseEventsNode = RootNode->FindChildNode("UseEvents");
        if (!UseEventsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract UseEvents node from file %s"), *XMLFilePath);

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
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UInventoryItemData* Plug = nullptr;
            return Plug;
        }

        TArray<FXmlNode*> UseEvents = UseEventsNode->GetChildrenNodes();
        //��������� ������ ���� �������, ���������� ��� ������� �� �����
        for (FXmlNode* EventNode : UseEvents) {
            if (!EventNode) {
                UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - EventNode from file %s is not valid"), *XMLFilePath);

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
                        UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
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
    //������������� ������ ��������� � ������� �������� ������
    return LoadDataFromXML<UInventoryItemData, UInventoryItemsList, UInventoryItemsPreparer>(
        this, ModuleName, "Inventory items", XMLFilePath, FileManager, InventoryItemsList, UploadingData, RecursionDepth);
}

UInventoryItemsPreparer::UInventoryItemsPreparer()
{
    InventoryItemsList = CreateDefaultSubobject<UInventoryItemsList>("InventoryItemsList");
}

//������� ��������� ������ ��� ���� ������ ��������� ���� �������
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
