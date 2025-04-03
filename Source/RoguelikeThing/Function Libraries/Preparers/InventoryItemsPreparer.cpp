// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/InventoryItemsPreparer.h"

DEFINE_LOG_CATEGORY(InventoryItemsPreparer);

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UInventoryItemsPreparer::CheckingDefaultInventoryItems()
{
    CheckingDefaultAbstractObjects(InventoryItemsList, "Inventory items");
}

//Функция загрузки данных об объекте из его xml файла
UInventoryItemData* UInventoryItemsPreparer::LoadObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth)
{
    auto UploadingData{ [](UInventoryItemsPreparer* Preparer, UInventoryItemData* InventoryItemData, FXmlNode* RootNode, FString FileName,
        UInventoryItemsList* InventoryItemsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth) {
        FXmlNode* MaximumAmountInStackNode = RootNode->FindChildNode("MaximumAmountInStack");
        if (!MaximumAmountInStackNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract MaximumAmountInStack node from file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            //Если файл был из модуля Default, то производится попытка его восстановления из исходного списка
            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    /* И после его восстановления, рекурсивно запускается эта же функция с целью ещё раз попытаться
                     * загрузить уже восстановленный файл. Переменная Preparer - это указатель на текущий экземпляр,
                     * так что рекурсия пойдёт из функции базового класса в функцию текущего дочернего экземпляра */
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    //Если же файл восстановить не удаётся, производится остановка выполнения программы
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FString StringMaximumAmountInStack = MaximumAmountInStackNode->GetContent();

        if (!StringMaximumAmountInStack.IsNumeric()) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Text in MaximumAmountInStack node from file %s is not numeric"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        int MaximumAmountInStack = FCString::Atoi(*StringMaximumAmountInStack);
        if (MaximumAmountInStack <= 0) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Number in MaximumAmountInStack node from file %s is less than one"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }
        InventoryItemData->MaximumAmountInStack = MaximumAmountInStack;

        FXmlNode* AllowedForUseOnExpeditionInteractionObjectsNode = RootNode->FindChildNode("AllowedForUseOnExpeditionInteractionObjects");
        if (!AllowedForUseOnExpeditionInteractionObjectsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract AllowedForUseOnExpeditionInteractionObjects node from file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FString StringAllowedForUseOnExpeditionInteractionObjects = AllowedForUseOnExpeditionInteractionObjectsNode->GetContent();
        //Здесь текст узла приводится к нижнему регистру, так что безопасно писать булевые переменные в xml файле как угодно
        StringAllowedForUseOnExpeditionInteractionObjects = StringAllowedForUseOnExpeditionInteractionObjects.ToLower();

        //Если текст узла не равен true или false, то это ошибка. Другие типы написания булевых переменных не слишком наглядны и не допускаются.
        if (!(StringAllowedForUseOnExpeditionInteractionObjects == "true" || StringAllowedForUseOnExpeditionInteractionObjects == "false")) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Text of the AllowedForUseOnExpeditionInteractionObjects node from file %s is not \"true\" or \"false\". You can write bool variables in any case, but you cannot denote them with numbers or anything else."), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        bool AllowedForUseOnExpeditionInteractionObjects = (StringAllowedForUseOnExpeditionInteractionObjects == "true" ? true : false);
        InventoryItemData->AllowedForUseOnExpeditionInteractionObjects = AllowedForUseOnExpeditionInteractionObjects;

        FXmlNode* UseEventsNode = RootNode->FindChildNode("UseEvents");
        if (!UseEventsNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract UseEvents node from file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        TArray<FXmlNode*> UseEvents = UseEventsNode->GetChildrenNodes();
        //Получение списка всех эвентов, вызываемых при нажатии по итему
        for (FXmlNode* EventNode : UseEvents) {
            if (!EventNode) {
                UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - EventNode from file %s is not valid"), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                        UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            FUseEvent UseEvent;
            InventoryItemData->UseEvents.Add(EventNode->GetTag(), UseEvent);
        }

        FXmlNode* ResourcesNode = RootNode->FindChildNode("Resources");
        if (!ResourcesNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract Resources node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FXmlNode* TexturesNode = ResourcesNode->FindChildNode("Textures");
        if (!TexturesNode) {
            UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Failed to extract Textures node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        TArray<FXmlNode*> Textures = TexturesNode->GetChildrenNodes();
        for (FXmlNode* Texture : Textures) {
            if (!Texture) {
                UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - Texture node from %s node from file %s is not valid"), *ResourcesNode->GetTag(), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UInventoryItemData>(InventoryItemData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(InventoryItemsPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Inventory items", InventoryItemsList)) {
                        UE_LOG(InventoryItemsPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(InventoryItemsPreparer, Error, TEXT("!!! An error occurred in the InventoryItemsPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            if (IsModDir) {
                TArray<FString> PathPeces;
                XMLFilePath.ParseIntoArray(PathPeces, TEXT("/"));

                InventoryItemData->TexturePaths.Add(Texture->GetTag(),
                    *FString(TEXT("Mods/") + PathPeces[PathPeces.Num() - 5] + "/Inventory items/" + ModuleName + TEXT("/textures/") +
                        InventoryItemData->id + "/" + Texture->GetContent()));

                UE_LOG(InventoryItemsPreparer, Error, TEXT("%s"), *FString(TEXT("Mods/") + PathPeces[PathPeces.Num() - 5] + "/Inventory items/" + ModuleName + TEXT("/textures/") +
                    InventoryItemData->id + "/" + Texture->GetContent()));
            }
            else {
                InventoryItemData->TexturePaths.Add(Texture->GetTag(),
                    *FString(TEXT("Data/Inventory items/") + ModuleName + TEXT("/textures/") + InventoryItemData->id + "/" + Texture->GetContent()));

                UE_LOG(InventoryItemsPreparer, Error, TEXT("%s"), *FString(TEXT("Data/Inventory items/") + ModuleName + TEXT("/textures/") + InventoryItemData->id + "/" + Texture->GetContent()));
            }
        }

        if (IsModDir) {
            TArray<FString> PathPeces;
            XMLFilePath.ParseIntoArray(PathPeces, TEXT("/"));

            InventoryItemData->ModuleLocalPath = "Mods/" + PathPeces[PathPeces.Num() - 5] + "/Inventory items/" + ModuleName + "/";
        }
        else {
            InventoryItemData->ModuleLocalPath = "Data/Inventory items/" + ModuleName + "/";
        }

        return InventoryItemData;
    } };
    //Вышеописанная лямбда передаётся в функцию базового класса
    return LoadDataFromXML<UInventoryItemData, UInventoryItemsList, UInventoryItemsPreparer>(
        this, ModuleName, "Inventory items", XMLFilePath, FileManager, InventoryItemsList, UploadingData, IsModDir, RecursionDepth);
}

UInventoryItemsPreparer::UInventoryItemsPreparer()
{
    InventoryItemsList = CreateDefaultSubobject<UInventoryItemsList>("InventoryItemsList");
}

//Функция получения данных обо всех итемах инвентаря всех модулей
void UInventoryItemsPreparer::GetAllInventoryItemsData(UInventoryItemsContainer* InventoryItemsContainer, TArray<FString> ModsDirWithInventoryItems)
{
    GetAllData<UInventoryItemsContainer, UInventoryItemData, UInventoryItemsPreparer>(
        InventoryItemsContainer, ModsDirWithInventoryItems, InventoryItemsList, "Inventory items", this);
}
