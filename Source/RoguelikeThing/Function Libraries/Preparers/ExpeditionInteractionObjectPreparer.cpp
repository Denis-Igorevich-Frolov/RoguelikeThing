// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ExpeditionInteractionObjectPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <XmlParser/Public/XmlFile.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <RoguelikeThing/SaveGame/ExpeditionInteractionObjectsSaver.h>

DEFINE_LOG_CATEGORY(ExpeditionInteractionObjectPreparer);

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UExpeditionInteractionObjectPreparer::CheckingDefaultExpeditionInteractionObjects()
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml"));
    //Если директории объектов взаимодействия по умолчанию нет, она создаётся
    if (!FileManager.DirectoryExists(*ModulePath)) {
        if (!FileManager.CreateDirectoryTree(*ModulePath)) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the CheckingDefaultExpeditionInteractionObjects function - Failed to create directory %s"), *ModulePath);
        }

        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("Directory %s has been created"), *ModulePath);
    }

    TArray<FString> XML_Files;
    expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.GenerateKeyArray(XML_Files);

    for (FString FileName : XML_Files) {
        FString FilePath = FString(ModulePath + "/" + FileName);

        //Если файла, связанного с объектом, который должен быть в модуле Default нет, то он восстанавливается из исходного списка
        if (!FileManager.FileExists(*FilePath)) {
            FString FileContent = *expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.Find(*FileName);
            
            if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the CheckingDefaultExpeditionInteractionObjects function - Failed to create file %s"), *FilePath);
            }

            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s from the Default module has been created"), *FilePath);
        }
    }
}

//Функция загрузки данных об объекте из его xml файла
UExpeditionInteractionObjectData* UExpeditionInteractionObjectPreparer::LoadExpeditionInteractionObjectFromXML(
    FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth)
{
    TArray<FString> PathPieces;
    //Переданный путь до xml файла разбивается на фрагменты, где последний элемент - это сам файл, а все предыдущие - папки, в которые он вложен
    XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);

    if (PathPieces.Num() == 0) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - The path to the xml file %s is empty or incorrect"), *XMLFilePath);
        return nullptr;
    }

    FString FileName = "";
    //Имя файла следует находить только для проверки на существование и валидность объекта из модуля Default
    if (ModuleName == "Default") {
        TArray<FString> FileNamePieces;
        //Берётся последний фрагмент пути до файла, который, непосредственно, будет полным именем самого файла, и разбивается на 2 части - имя и расширение
        PathPieces.Last().ParseIntoArray(FileNamePieces, TEXT("."), false);

        if (FileNamePieces.IsValidIndex(0)) {
            /* Из всего пути до файла вычленяется имя файла без расширения, которое используется как индекс
             * для поиска и, при необходимости, восстановления данных об объекте из модуля Default */
            FileName = FileNamePieces[0];
        }
        else {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - File name %s has no extension or is incorrect"), *XMLFilePath);
            return nullptr;
        }
    }

    UPROPERTY()
    UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = NewObject<UExpeditionInteractionObjectData>();

    FXmlFile* XmlFile = new FXmlFile(*XMLFilePath);
    if (!XmlFile) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to create file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        //Если файл был из модуля Default, то производится попытка его восстановления из исходного списка
        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                //И после его восстановления, рекурсивно запускается эта же функция с целью ещё раз попытаться загрузить уже восстановленный файл
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                //Если же файл восстановить не удаётся, производится остановка выполнения программы
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    if (!XmlFile->IsValid()) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - xml code from file %s is not valid"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FXmlNode* RootNode = XmlFile->GetRootNode();
    if (!RootNode) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract RootNode from file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FXmlNode* IdNode = RootNode->FindChildNode("id");
    if (!IdNode) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract id node from file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString id = IdNode->GetContent();
    if (id == "") {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Id node from file %s is empty"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }
    ExpeditionInteractionObjectData->id = id;

    FXmlNode* NameNode = RootNode->FindChildNode("Name");
    if (!NameNode) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract Name node from file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString Name = NameNode->GetContent();
    if (Name == "") {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Name node from file %s is empty"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }
    ExpeditionInteractionObjectData->Name = Name;

    FXmlNode* InteractionTextNode = RootNode->FindChildNode("InteractionText");
    if (!InteractionTextNode) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract InteractionText node from file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString InteractionText = InteractionTextNode->GetContent();
    if (InteractionText == "") {
        UE_LOG(ExpeditionInteractionObjectPreparer, Warning, TEXT("Warning in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - The InteractionText node from file %s is empty, which is undesirable"), *XMLFilePath);
    }
    ExpeditionInteractionObjectData->InteractionText = InteractionText;

    FXmlNode* TermsOfInteractionsNode = RootNode->FindChildNode("TermsOfInteractions");
    if (!TermsOfInteractionsNode) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract TermsOfInteractions node from file %s"), *XMLFilePath);

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName)) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    TArray<FXmlNode*> TermsOfInteractions = TermsOfInteractionsNode->GetChildrenNodes();
    for (FXmlNode* InteractionNode : TermsOfInteractions) {
        if (!InteractionNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - InteractionNode from file %s is not valid"), *XMLFilePath);

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (RestoringDefaultFileByName(FileName)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                    return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            return nullptr;
        }

        FXmlNode* EventsText = InteractionNode->FindChildNode("EventsText");
        if (!EventsText) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract EventsText node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (RestoringDefaultFileByName(FileName)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                    return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            return nullptr;
        }

        FInteractionCondition InteractionCondition;
        InteractionCondition.InteractionText = EventsText->GetContent();

        FXmlNode* EventsNode = InteractionNode->FindChildNode("Events");
        if (!EventsNode) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Failed to extract Events node from %s node from file %s"), *InteractionNode->GetTag(), *XMLFilePath);

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                if (RestoringDefaultFileByName(FileName)) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                    return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                }
                else {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                    FGenericPlatformMisc::RequestExit(false);
                }
            }

            return nullptr;
        }

        TArray<FXmlNode*> Events = EventsNode->GetChildrenNodes();
        for (FXmlNode* Event : Events) {
            if (!Event) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - Event node from %s node from file %s is not valid"), *InteractionNode->GetTag(), *XMLFilePath);

                if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                    ExpeditionInteractionObjectData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
                    if (RestoringDefaultFileByName(FileName)) {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *XMLFilePath);
                        return LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager, ++RecursionDepth);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the LoadExpeditionInteractionObjectFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                        FGenericPlatformMisc::RequestExit(false);
                    }
                }

                return nullptr;
            }

            FInteractionEvent InteractionEvent;
            InteractionCondition.InteractionEvents.Add(Event->GetTag(), InteractionEvent);
        }

        ExpeditionInteractionObjectData->TermsOfInteractions.Add(InteractionNode->GetTag(), InteractionCondition);
    }

    if (XmlFile)
        delete XmlFile;

    return ExpeditionInteractionObjectData;
}

//Функция восстановления файла из моодуля Default по его имени (без расширения)
bool UExpeditionInteractionObjectPreparer::RestoringDefaultFileByName(FString Name)
{
    Name += ".xml";

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml/"));
    FString FilePath = ModulePath + Name;
    FString FileContent = *expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.Find(*Name);

    if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the ConvertRelativePathToFull function - Failed to create file %s"), *FilePath);
        return false;
    }

    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("File %s has been restored"), *FilePath);
    return true;
}

//Функция получения данных обо всех объектах взаимодействия всех модулей
void UExpeditionInteractionObjectPreparer::GetAllExpeditionInteractionObjectsData(TMap<FString, UExpeditionInteractionObjectData*>& InteractionObjectsDataArray)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [&InteractionObjectsDataArray, this]() {
        //Сначала проверяется не повреждены ли данные модуля Default
        CheckingDefaultExpeditionInteractionObjects();

        //Получение списка папок всех модулей
        TArray<FString> Dirs;
        FString StartDirPath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects"));
        FFileManagerGeneric::Get().FindFilesRecursive(Dirs, *StartDirPath, TEXT("*"), false, true);

        TArray<FString> ModuleNames;
        //Так как функция FindFilesRecursive находит все вложенные директории, а не только корневые, все остальные стоит отсеять
        for (FString Dir : Dirs) {
            //Путь до директории разбивается на массив последовательно вложенных папок
            TArray<FString> DirPieces;
            Dir.ParseIntoArray(DirPieces, TEXT("/"), false);

            //И проверяется на каком месте находится корень для всех модулей объектов взаимодействия
            int SequentialDirectoryNumber = 0;
            DirPieces.Find("Expedition interaction objects", SequentialDirectoryNumber);

            /* Отсееваются некорневые директории по длинне пути. Если директория корневая,
             * то её длинна должна быть равна последовательному номеру корня общего для всех
             * модулей + 1. Также стоит учеть, что SequentialDirectoryNumber - это индекс,
             * которому до последовательного номера следует прибавить ещё 1 */
            if (DirPieces.Num() == SequentialDirectoryNumber + 2 && DirPieces.IsValidIndex(SequentialDirectoryNumber + 1)) {
                ModuleNames.Add(*DirPieces[SequentialDirectoryNumber + 1]);
            }
        }

        IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
        for (FString ModuleName : ModuleNames) {
            UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("Loading of the expedition interaction objects module %s has begun"), *ModuleName);

            FString ModuleXMLDir = FString(StartDirPath + "/" + ModuleName + "/xml");

            TArray<FString> XMLFilesPaths;
            FileManager.FindFiles(XMLFilesPaths, *ModuleXMLDir, TEXT("xml"));

            UExpeditionInteractionObjectsSaver* InteractionObjectsSaver = NewObject<UExpeditionInteractionObjectsSaver>();

            FString ExpeditionInteractionObjectsSaverFilePath = FPaths::ProjectDir() + "Data/Expedition interaction objects/" + ModuleName + "/sav/" + ModuleName + ".sav";
            bool ExpeditionInteractionObjectsSaverFileExist = FileManager.FileExists(*ExpeditionInteractionObjectsSaverFilePath);

            //Если существует сериализованный файл данных о модуле, он загружается
            if (ExpeditionInteractionObjectsSaverFileExist) {
                InteractionObjectsSaver->LoadBinArray(ExpeditionInteractionObjectsSaverFilePath);
            }

            //Идёт проверка не изменилось ли количество xml файлов в директории модуля и не изменился ли их хеш
            if (InteractionObjectsSaver && XMLFilesPaths.Num() == InteractionObjectsSaver->GetBinArraySize() && InteractionObjectsSaver->CheckHashChange()) {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("Loading will be done from a save file %s with serialized data"), *ExpeditionInteractionObjectsSaverFilePath);
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, FString(ModuleName + ".sav file loading"));

                //Если все файлы остались неизменными с момента сериализации, то загрузка объектов ведётся напрямую из сохранённых бинарных данных
                TMap<FString, UExpeditionInteractionObjectData*> ExpeditionInteractionObjectsData = InteractionObjectsSaver->GetExpeditionInteractionObjectsData();
                for (FString XMLFilePath : XMLFilesPaths) {
                    UPROPERTY()
                    UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = *ExpeditionInteractionObjectsData.Find(XMLFilePath);
                    if (ExpeditionInteractionObjectData) {
                        InteractionObjectsDataArray.Add(ExpeditionInteractionObjectData->id, ExpeditionInteractionObjectData);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the GetAllExpeditionInteractionObjectsData function - ExpeditionInteractionObjectData is not valid"));
                        
                        TArray<FString> PathPieces;
                        XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString("Object " + PathPieces.Last().Left(4) + " could not be loaded"));
                    }
                }
            }
            else {
                UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("Loading will be done from an array of xml files"));

                //Если файлы менялись, то загрузка производится из xml файлов с их последующим сохранениев в новый sav файл
                InteractionObjectsSaver->ClearArray();
                for (FString XMLFilePath : XMLFilesPaths) {
                    UE_LOG(ExpeditionInteractionObjectPreparer, Log, TEXT("Loading will be done from a xml file %s"), *XMLFilePath);

                    TArray<FString> PathPieces;
                    XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                    GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, FString(PathPieces.Last() + " file loading"));

                    UPROPERTY()
                    UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = LoadExpeditionInteractionObjectFromXML(ModuleName, XMLFilePath, FileManager);

                    if (ExpeditionInteractionObjectData) {
                        InteractionObjectsSaver->AddExpeditionInteractionObjectDataToBinArray(ExpeditionInteractionObjectData, XMLFilePath);

                        InteractionObjectsDataArray.Add(ExpeditionInteractionObjectData->id, ExpeditionInteractionObjectData);
                    }
                    else {
                        UE_LOG(ExpeditionInteractionObjectPreparer, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectPreparer class in the GetAllExpeditionInteractionObjectsData function - ExpeditionInteractionObjectData is not valid"));
                        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString("Object " + PathPieces.Last().Left(4) + " could not be loaded"));
                    }
                }

                //Заново сгенерированный массив данных обо всех объектах интеракции данного модуля сериализуется и сохраняется в sav файл
                InteractionObjectsSaver->SaveBinArray(ExpeditionInteractionObjectsSaverFilePath);
            }
        }
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Expedition interaction objects data loading complite"));
        });
}
