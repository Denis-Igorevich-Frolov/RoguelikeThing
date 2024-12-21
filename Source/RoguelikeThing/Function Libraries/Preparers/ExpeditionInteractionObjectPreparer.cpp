// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ExpeditionInteractionObjectPreparer.h"
#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"
#include <IPlatformFileSandboxWrapper.h>
#include <HAL/FileManagerGeneric.h>

void UExpeditionInteractionObjectPreparer::CheckingDefaultExpeditionInteractionObjects()
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
    ExpeditionInteractionObjectsList expeditionInteractionObjectsList;

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("/Data/Expedition interaction objects/Default/xml"));
    if (!FileManager.DirectoryExists(*ModulePath)) {
        if (FileManager.CreateDirectoryTree(*ModulePath)) {
            UE_LOG(LogTemp, Error, TEXT("!!!1"));
        }
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
        }
    }
}

TMap<FString, UExpeditionInteractionObjectData*> UExpeditionInteractionObjectPreparer::GetAllExpeditionInteractionObjectsData()
{
    CheckingDefaultExpeditionInteractionObjects();

    TArray<FString> Dirs;
    FString StartDirPath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("/Data/Expedition interaction objects"));
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
            break;
        }

        if (!FileManager.DirectoryExists(*ModuleSAVDir)) {
            if (FileManager.CreateDirectoryTree(*ModuleSAVDir)) {
                UE_LOG(LogTemp, Error, TEXT("!!!3"));
            }
        }
    }

    //IFileManager& FileManager = IFileManager::Get();
    //TArray<FString> Files;
    //FString Path = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("/Data/Expedition interaction objects"));
    //FString Extention ("");
    //FileManager.FindFiles(Files, *Path, *Extention);

    //for (FString s : Files) {
    //        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, s);
    //}

    return TMap<FString, UExpeditionInteractionObjectData*>();
}
