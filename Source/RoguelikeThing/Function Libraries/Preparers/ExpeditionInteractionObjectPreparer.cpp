// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ExpeditionInteractionObjectPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <XmlParser/Public/XmlFile.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <RoguelikeThing/SaveGame/ExpeditionInteractionObjectsSaver.h>

void UExpeditionInteractionObjectPreparer::CheckingDefaultExpeditionInteractionObjects()
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml"));
    if (!FileManager.DirectoryExists(*ModulePath)) {
        if (!FileManager.CreateDirectoryTree(*ModulePath)) {
            UE_LOG(LogTemp, Error, TEXT("!!!1"));
        }

        UE_LOG(LogTemp, Log, TEXT("Default/mxl created"));
    }

    TArray<FString> XML_Files;
    expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.GenerateKeyArray(XML_Files);

    for (FString FileName : XML_Files) {
        FString FilePath = FString(ModulePath + "/" + FileName);

        if (!FileManager.FileExists(*FilePath)) {
            FString FileContent = *expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.Find(*FileName);
            
            if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
                UE_LOG(LogTemp, Error, TEXT("!!!2"));
            }

            UE_LOG(LogTemp, Log, TEXT("%s created"), *FileName);
        }
    }
}

UExpeditionInteractionObjectData* UExpeditionInteractionObjectPreparer::LoadExpeditionInteractionObject(
    FString ModuleName, FString XMLFilePath, FString ModuleSAVDir, IPlatformFile& FileManager)
{
    TArray<FString> PathPieces;
    XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);

    if (PathPieces.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("!!!4"));
        return nullptr;
    }

    TArray<FString> FileNamePieces;
    PathPieces.Last().ParseIntoArray(FileNamePieces, TEXT("."), false);

    FString FileName;
    if (FileNamePieces.IsValidIndex(0)) {
        FileName = FileNamePieces[0];
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("!!!5"));
        return nullptr;
    }

    UPROPERTY()
    UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = NewObject<UExpeditionInteractionObjectData>();

    FXmlFile* XmlFile = new FXmlFile(*XMLFilePath);
    if (!XmlFile) {
        UE_LOG(LogTemp, Error, TEXT("!!!6"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }

    FXmlNode* RootNode = XmlFile->GetRootNode();
    if (!RootNode) {
        UE_LOG(LogTemp, Error, TEXT("!!!7"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }

    FXmlNode* IdNode = RootNode->FindChildNode("id");
    if (!IdNode) {
        UE_LOG(LogTemp, Error, TEXT("!!!8"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    FString id = IdNode->GetContent();
    if (id == "") {
        UE_LOG(LogTemp, Error, TEXT("!!!9"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    ExpeditionInteractionObjectData->id = id;

    FXmlNode* NameNode = RootNode->FindChildNode("Name");
    if (!NameNode) {
        UE_LOG(LogTemp, Error, TEXT("!!!10"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    FString Name = NameNode->GetContent();
    if (id == "") {
        UE_LOG(LogTemp, Error, TEXT("!!!11"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    ExpeditionInteractionObjectData->Name = Name;

    FXmlNode* InteractionTextNode = RootNode->FindChildNode("InteractionText");
    if (!InteractionTextNode) {
        UE_LOG(LogTemp, Error, TEXT("!!!10"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    FString InteractionText = InteractionTextNode->GetContent();
    if (id == "") {
        UE_LOG(LogTemp, Error, TEXT("!!!11"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }
    ExpeditionInteractionObjectData->InteractionText = InteractionText;

    FXmlNode* TermsOfInteractionsNode = RootNode->FindChildNode("TermsOfInteractions");
    if (!TermsOfInteractionsNode) {
        UE_LOG(LogTemp, Error, TEXT("!!!12"));

        if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
            ExpeditionInteractionObjectData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            RestoringDefaultFileByName(FileName);
            return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
        }

        return nullptr;
    }

    TArray<FXmlNode*> TermsOfInteractions = TermsOfInteractionsNode->GetChildrenNodes();
    for (FXmlNode* InteractionNode : TermsOfInteractions) {
        if (!InteractionNode) {
            UE_LOG(LogTemp, Error, TEXT("!!!13"));

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                RestoringDefaultFileByName(FileName);
                return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
            }

            return nullptr;
        }

        FXmlNode* EventsText = InteractionNode->FindChildNode("EventsText");
        if (!EventsText) {
            UE_LOG(LogTemp, Error, TEXT("!!!14"));

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                RestoringDefaultFileByName(FileName);
                return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
            }

            return nullptr;
        }

        FInteractionCondition InteractionCondition;
        InteractionCondition.InteractionText = EventsText->GetContent();

        FXmlNode* EventsNode = InteractionNode->FindChildNode("Events");
        if (!EventsNode) {
            UE_LOG(LogTemp, Error, TEXT("!!!15"));

            if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                ExpeditionInteractionObjectData->MarkPendingKill();
            }

            if (ModuleName == "Default") {
                RestoringDefaultFileByName(FileName);
                return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
            }

            return nullptr;
        }

        TArray<FXmlNode*> Events = EventsNode->GetChildrenNodes();
        for (FXmlNode* Event : Events) {
            if (!Event) {
                UE_LOG(LogTemp, Error, TEXT("!!!16"));

                if (ExpeditionInteractionObjectData && ExpeditionInteractionObjectData->IsValidLowLevel()) {
                    ExpeditionInteractionObjectData->MarkPendingKill();
                }

                if (ModuleName == "Default") {
                    RestoringDefaultFileByName(FileName);
                    return LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);
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

void UExpeditionInteractionObjectPreparer::RestoringDefaultFileByName(FString Name)
{
    Name += ".xml";

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml/"));
    FString FilePath = ModulePath + Name;
    FString FileContent = *expeditionInteractionObjectsList.ExpeditionInteractionObjectsXMLs.Find(*Name);

    if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
        UE_LOG(LogTemp, Error, TEXT("!!!Restoring"));
    }

    UE_LOG(LogTemp, Log, TEXT("%s Restoring created"), *FilePath);
}

void UExpeditionInteractionObjectPreparer::GetAllExpeditionInteractionObjectsData(TMap<FString, UExpeditionInteractionObjectData*>& InteractionObjectsDataArray)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [&InteractionObjectsDataArray, this]() {
        CheckingDefaultExpeditionInteractionObjects();

        TArray<FString> Dirs;
        FString StartDirPath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects"));
        FFileManagerGeneric::Get().FindFilesRecursive(Dirs, *StartDirPath, TEXT("*"), false, true);

        TArray<FString> ModuleNames;
        for (FString Dir : Dirs) {
            TArray<FString> DirPieces;
            Dir.ParseIntoArray(DirPieces, TEXT("/"), false);

            int i = 0;
            DirPieces.Find("Expedition interaction objects", i);

            //i - индекс и ещё +1
            if (DirPieces.Num() == i + 2 && DirPieces.IsValidIndex(i + 1)) {
                ModuleNames.Add(*DirPieces[i + 1]);
            }
        }

        IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
        for (FString ModuleName : ModuleNames) {
            FString ModuleXMLDir = FString(StartDirPath + "/" + ModuleName + "/xml");
            FString ModuleSAVDir = FString(StartDirPath + "/" + ModuleName + "/sav");
            if (!FileManager.DirectoryExists(*ModuleXMLDir)) {
                UE_LOG(LogTemp, Log, TEXT("%s is empty"), *ModuleXMLDir);
                break;
            }

            if (!FileManager.DirectoryExists(*ModuleSAVDir)) {
                if (!FileManager.CreateDirectoryTree(*ModuleSAVDir)) {
                    UE_LOG(LogTemp, Error, TEXT("!!!3"));
                }
                UE_LOG(LogTemp, Log, TEXT("%s created"), *ModuleSAVDir);
            }

            TArray<FString> XMLFilesPaths;
            FileManager.FindFiles(XMLFilesPaths, *ModuleXMLDir, TEXT("xml"));

            AsyncTask(ENamedThreads::GameThread, [&InteractionObjectsDataArray, ModuleName, XMLFilesPaths, ModuleSAVDir, &FileManager, this]() {
                UExpeditionInteractionObjectsSaver* ExpeditionInteractionObjectsSaver = NewObject<UExpeditionInteractionObjectsSaver>();

                FString ExpeditionInteractionObjectsSaverFilePath = FPaths::ProjectDir() + "Data/Expedition interaction objects/" + ModuleName + "/sav/" + ModuleName + ".sav";
                bool ExpeditionInteractionObjectsSaverFileExist = FileManager.FileExists(*ExpeditionInteractionObjectsSaverFilePath);

                if (ExpeditionInteractionObjectsSaverFileExist) {
                    ExpeditionInteractionObjectsSaver->LoadBinArray(ExpeditionInteractionObjectsSaverFilePath);
                }

                AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ExpeditionInteractionObjectsSaverFileExist, ExpeditionInteractionObjectsSaver, XMLFilesPaths,
                    &InteractionObjectsDataArray, ModuleName, ModuleSAVDir, &FileManager, ExpeditionInteractionObjectsSaverFilePath, this]() {
                        if (ExpeditionInteractionObjectsSaverFileExist && XMLFilesPaths.Num() == ExpeditionInteractionObjectsSaver->GetBinArraySize() &&
                            ExpeditionInteractionObjectsSaver->CheckHashChange()) {
                            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, TEXT("SAV load"));

                            TMap<FString, UExpeditionInteractionObjectData*> ExpeditionInteractionObjectsData = ExpeditionInteractionObjectsSaver->GetExpeditionInteractionObjectsData();
                            for (FString XMLFilePath : XMLFilesPaths) {
                                UPROPERTY()
                                UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = *ExpeditionInteractionObjectsData.Find(XMLFilePath);
                                if (ExpeditionInteractionObjectData) {
                                    InteractionObjectsDataArray.Add(ExpeditionInteractionObjectData->id, ExpeditionInteractionObjectData);
                                }
                                else {
                                    UE_LOG(LogTemp, Error, TEXT("!ExpeditionInteractionObjectData"));
                                }
                            }
                        }
                        else {
                            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Purple, TEXT("XML load"));

                            ExpeditionInteractionObjectsSaver->ClearArray();
                            for (FString XMLFilePath : XMLFilesPaths) {
                                UPROPERTY()
                                UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = LoadExpeditionInteractionObject(ModuleName, XMLFilePath, ModuleSAVDir, FileManager);

                                if (ExpeditionInteractionObjectData) {
                                    ExpeditionInteractionObjectsSaver->AddExpeditionInteractionObjectDataToBinArray(ExpeditionInteractionObjectData, XMLFilePath);

                                    InteractionObjectsDataArray.Add(ExpeditionInteractionObjectData->id, ExpeditionInteractionObjectData);
                                }
                                else {
                                    UE_LOG(LogTemp, Error, TEXT("!ExpeditionInteractionObjectData"));
                                }
                            }

                            ExpeditionInteractionObjectsSaver->SaveBinArray(ExpeditionInteractionObjectsSaverFilePath);
                        }
                    });
                });

            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Expedition interaction objects data loading complite"));
        }
        });
}
