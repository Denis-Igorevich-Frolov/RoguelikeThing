﻿// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ExpeditionInteractionObjectPreparer.h"

DEFINE_LOG_CATEGORY(ExpeditionInteractionObjectPreparer);

UExpeditionInteractionObjectPreparer::UExpeditionInteractionObjectPreparer()
{
    ExpeditionInteractionObjectsList = CreateDefaultSubobject<UExpeditionInteractionObjectsList>("ExpeditionInteractionObjectsList");
}

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UExpeditionInteractionObjectPreparer::CheckingDefaultExpeditionInteractionObjects()
{
    CheckingDefaultAbstractObjects(ExpeditionInteractionObjectsList, "Expedition interaction objects");
}

//Функция загрузки данных об объекте из его xml файла
UExpeditionInteractionObjectData* UExpeditionInteractionObjectPreparer::LoadObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth)
{
    //Подготавливается лямбда, которая выполнит ту часть загрузки данных базового класса, которая специфична только для данных этого класса
    auto UploadingData{ [](UExpeditionInteractionObjectPreparer* Preparer, UExpeditionInteractionObjectData* ExpeditionInteractionObjectData,
        FXmlNode* RootNode, FString FileName, UExpeditionInteractionObjectsList* ExpeditionInteractionObjectsList, FString ModuleName,
        FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth) {

        if (RecursionDepth > 100) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Too many unsuccessful file recovery attempts while trying to load file %s. The Default module files are corrupted. Please check the integrity of your data or reinstall the game."), *XMLFilePath);
            FGenericPlatformMisc::RequestExit(false);
        }

        FXmlNode* InteractionTextNode = RootNode->FindChildNode("InteractionText");
        if (!InteractionTextNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract InteractionText node from file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            //Если файл был из модуля Default, то производится попытка его восстановления из исходного списка
            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    /* И после его восстановления, рекурсивно запускается эта же функция с целью ещё раз попытаться
                     * загрузить уже восстановленный файл. Переменная Preparer - это указатель на текущий экземпляр,
                     * так что рекурсия пойдёт из функции базового класса в функцию текущего дочернего экземпляра */
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    //Если же файл восстановить не удаётся, производится остановка выполнения программы
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FString InteractionText = InteractionTextNode->GetContent();
        if (InteractionText == "") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Warning, TEXT("Warning in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - The InteractionText node from file %s is empty, which is undesirable"), *XMLFilePath);
        }
        ExpeditionInteractionObjectData->InteractionText = InteractionText;

        FXmlNode* TermsOfInteractionsNode = RootNode->FindChildNode("TermsOfInteractions");
        if (!TermsOfInteractionsNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract TermsOfInteractions node from file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        //Получение списка всех груп условий взаимодействия с объектом
        TArray<FXmlNode*> TermsOfInteractions = TermsOfInteractionsNode->GetChildrenNodes();
        for (FXmlNode* InteractionNode : TermsOfInteractions) {
            if (!InteractionNode) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - InteractionNode from file %s is not valid"), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            FInteractionCondition InteractionCondition;

            FXmlNode* ConditionsNode = InteractionNode->FindChildNode("Conditions");
            if (!ConditionsNode) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract Conditions node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            //Сборка всех подусловий в одно единое множество условий взаимодействия с объектом
            TArray<FXmlNode*> Conditions = ConditionsNode->GetChildrenNodes();
            for (FXmlNode* Condition : Conditions) {
                InteractionCondition.ConditionsText.Add(Condition->GetContent());
            }

            //Получение текста, которое будет выведено на экран при выполнении вышеописанных условий
            FXmlNode* EventsText = InteractionNode->FindChildNode("EventsText");
            if (!EventsText) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract EventsText node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            InteractionCondition.InteractionText = EventsText->GetContent();

            FXmlNode* EventsNode = InteractionNode->FindChildNode("Events");
            if (!EventsNode) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract Events node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

                Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

                if (ModuleName == "Default" && !IsModDir) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                        return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }
                else {
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            //Получение списка эвентов, которые будут выполнены при выполнении вышеописанных условий
            TArray<FXmlNode*> Events = EventsNode->GetChildrenNodes();
            for (FXmlNode* Event : Events) {
                if (!Event) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Event node from %s node from file %s is not valid"), *InteractionNode->GetTag(), *XMLFilePath);

                    Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

                    if (ModuleName == "Default" && !IsModDir) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                        if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                            return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                        }
                        else {
                            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                            FGenericPlatformMisc::RequestExit(false);
                        }
                    }
                    else {
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                InteractionCondition.InteractionEventsText.Add(Event->GetContent());
            }

            ExpeditionInteractionObjectData->TermsOfInteractions.Add(InteractionNode->GetTag(), InteractionCondition);
        }

        FXmlNode* ResourcesNode = RootNode->FindChildNode("Resources");
        if (!ResourcesNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract Resources node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FXmlNode* TexturesNode = ResourcesNode->FindChildNode("Textures");
        if (!TexturesNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to extract Textures node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        if (!Preparer->LoadTextures(ResourcesNode, ExpeditionInteractionObjectData, XMLFilePath, IsModDir, ModuleName,
            "Expedition interaction objects")) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to load textures specified in file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }
            else {
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        if (!Preparer->LoadSounds(ResourcesNode, ExpeditionInteractionObjectData, XMLFilePath, IsModDir, ModuleName,
            "Expedition interaction objects")) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - Failed to load sounds specified in file %s"), *XMLFilePath);

            Preparer->EmergencyResetOfPointers<UExpeditionInteractionObjectData>(ExpeditionInteractionObjectData);

            if (ModuleName == "Default" && !IsModDir) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (Preparer->RestoringDefaultFileByName(FileName, "Expedition interaction objects", ExpeditionInteractionObjectsList)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                    return Preparer->LoadObjectFromXML(ModuleName, XMLFilePath, FileManager, IsModDir, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
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

            ExpeditionInteractionObjectData->ModuleLocalPath = "Mods/" + PathPeces[PathPeces.Num() - 5] + "/Expedition interaction objects/" + ModuleName + "/";
        }
        else {
            ExpeditionInteractionObjectData->ModuleLocalPath = "Data/Expedition interaction objects/" + ModuleName + "/";
        }

        return ExpeditionInteractionObjectData;
        } };
    //Вышеописанная лямбда передаётся в функцию базового класса
    return LoadDataFromXML<UExpeditionInteractionObjectData, UExpeditionInteractionObjectsList, UExpeditionInteractionObjectPreparer>(
        this, ModuleName, "Expedition interaction objects", XMLFilePath, FileManager, ExpeditionInteractionObjectsList, UploadingData, IsModDir, RecursionDepth);
}

//Функция получения данных обо всех объектах взаимодействия всех модулей
void UExpeditionInteractionObjectPreparer::GetAllObjectsData(
    UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, TArray<FString> ModsDirWithInteractionObjects)
{
    auto AdditionaCalculations{ [](UExpeditionInteractionObjectContainer* Cont) {
        TMap<FString, UExpeditionInteractionObjectData*> ExpeditionInteractionObjectsData = Cont->GetAllObjects();
        TArray<UExpeditionInteractionObjectData*> DataArray;
        ExpeditionInteractionObjectsData.GenerateValueArray(DataArray);

        for (UExpeditionInteractionObjectData* Data : DataArray) {
            TArray<FString> InteractionConditionsKeys;
            Data->TermsOfInteractions.GenerateKeyArray(InteractionConditionsKeys);

            for (FString Key : InteractionConditionsKeys) {
                FInteractionCondition* Condition = Data->TermsOfInteractions.Find(Key);
                Condition->PrepareConditions(Cont);
                Condition->PrepareEvents(Cont);
            }
        }
        } };

    GetAllData<UExpeditionInteractionObjectContainer, UExpeditionInteractionObjectData, UExpeditionInteractionObjectPreparer>(
        ExpeditionInteractionObjectContainer, ModsDirWithInteractionObjects, ExpeditionInteractionObjectsList, "Expedition interaction objects", this, AdditionaCalculations);
}
