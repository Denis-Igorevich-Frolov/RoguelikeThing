// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ExpeditionInteractionObjectPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <XmlParser/Public/XmlFile.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <RoguelikeThing/SaveGame/DataSaver.h>

DEFINE_LOG_CATEGORY(ExpeditionInteractionObjectPreparer);

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UExpeditionInteractionObjectPreparer::CheckingDefaultExpeditionInteractionObjects()
{
    CheckingDefaultAbstracts(ExpeditionInteractionObjectsList, "Expedition interaction objects");
}

//Функция загрузки данных об объекте из его xml файла
UExpeditionInteractionObjectData* UExpeditionInteractionObjectPreparer::LoadObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth)
{
    auto UploadingData{ [](UExpeditionInteractionObjectPreparer* Preparer, UExpeditionInteractionObjectData* ExpeditionInteractionObjectData,
        FXmlNode* RootNode, FString FileName, UExpeditionInteractionObjectsList* ExpeditionInteractionObjectsList, FString ModuleName,
        FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth) {
        FXmlNode* InteractionTextNode = RootNode->FindChildNode("InteractionText");
        if (!InteractionTextNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract InteractionText node from file %s"), *XMLFilePath);

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                if (ExpeditionInteractionObjectData->IsRooted())
                    ExpeditionInteractionObjectData->RemoveFromRoot();

                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UExpeditionInteractionObjectData* Plug = nullptr;
            return Plug;
        }

        FString InteractionText = InteractionTextNode->GetContent();
        if (InteractionText == "") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Warning, TEXT("Warning in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - The InteractionText node from file %s is empty, which is undesirable"), *XMLFilePath);
        }
        ExpeditionInteractionObjectData->InteractionText = InteractionText;

        FXmlNode* TermsOfInteractionsNode = RootNode->FindChildNode("TermsOfInteractions");
        if (!TermsOfInteractionsNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract TermsOfInteractions node from file %s"), *XMLFilePath);

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                if (ExpeditionInteractionObjectData->IsRooted())
                    ExpeditionInteractionObjectData->RemoveFromRoot();

                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            UExpeditionInteractionObjectData* Plug = nullptr;
            return Plug;
        }

        TArray<FXmlNode*> TermsOfInteractions = TermsOfInteractionsNode->GetChildrenNodes();
        for (FXmlNode* InteractionNode : TermsOfInteractions) {
            if (!InteractionNode) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - InteractionNode from file %s is not valid"), *XMLFilePath);

                if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                    if (ExpeditionInteractionObjectData->IsRooted())
                        ExpeditionInteractionObjectData->RemoveFromRoot();

                    ExpeditionInteractionObjectData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                UExpeditionInteractionObjectData* Plug = nullptr;
                return Plug;
            }

            FXmlNode* EventsText = InteractionNode->FindChildNode("EventsText");
            if (!EventsText) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract EventsText node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

                if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                    if (ExpeditionInteractionObjectData->IsRooted())
                        ExpeditionInteractionObjectData->RemoveFromRoot();

                    ExpeditionInteractionObjectData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                UExpeditionInteractionObjectData* Plug = nullptr;
                return Plug;
            }

            FInteractionCondition InteractionCondition;
            InteractionCondition.InteractionText = EventsText->GetContent();

            FXmlNode* EventsNode = InteractionNode->FindChildNode("Events");
            if (!EventsNode) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract Events node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

                if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                    if (ExpeditionInteractionObjectData->IsRooted())
                        ExpeditionInteractionObjectData->RemoveFromRoot();

                    ExpeditionInteractionObjectData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                UExpeditionInteractionObjectData* Plug = nullptr;
                return Plug;
            }

            TArray<FXmlNode*> Events = EventsNode->GetChildrenNodes();
            for (FXmlNode* Event : Events) {
                if (!Event) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Event node from %s node from file %s is not valid"), *InteractionNode->GetTag(), *XMLFilePath);

                    if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                        if (ExpeditionInteractionObjectData->IsRooted())
                            ExpeditionInteractionObjectData->RemoveFromRoot();

                        ExpeditionInteractionObjectData->MarkPendingKill();
                    }

                    if (ModuleName == "Default") {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                        if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                            return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                        }
                        else {
                            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                            FGenericPlatformMisc::RequestExit(false);
                        }
                    }

                    UExpeditionInteractionObjectData* Plug = nullptr;
                    return Plug;
                }

                FInteractionEvent InteractionEvent;
                InteractionCondition.InteractionEvents.Add(Event->GetTag(), InteractionEvent);
            }

            ExpeditionInteractionObjectData->TermsOfInteractions.Add(InteractionNode->GetTag(), InteractionCondition);
        }

        return ExpeditionInteractionObjectData;
        } };
    return LoadDataFromXML<UExpeditionInteractionObjectData, UExpeditionInteractionObjectsList, UExpeditionInteractionObjectPreparer>(
        this, ModuleName, "Expedition interaction objects", XMLFilePath, FileManager, ExpeditionInteractionObjectsList, UploadingData, RecursionDepth);
}

UExpeditionInteractionObjectPreparer::UExpeditionInteractionObjectPreparer()
{
    ExpeditionInteractionObjectsList = CreateDefaultSubobject<UExpeditionInteractionObjectsList>("ExpeditionInteractionObjectsList");
}

//Функция получения данных обо всех объектах взаимодействия всех модулей
void UExpeditionInteractionObjectPreparer::GetAllObjectsData(
    UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, TArray<FString> ModsDirWithInteractionObjects)
{
    GetAllData<UExpeditionInteractionObjectContainer, UExpeditionInteractionObjectData, UExpeditionInteractionObjectPreparer>(
        ExpeditionInteractionObjectContainer, ModsDirWithInteractionObjects, ExpeditionInteractionObjectsList, "Expedition interaction objects", this);
}

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
            FExpeditionInteractionObjectSubCategory* SubCategory  = Category->InteractionObjectsSubCategorysArray.Find(SubCategoryName);
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
