// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/MyFileManager.h"

DEFINE_LOG_CATEGORY(MyFileManager);

bool UMyFileManager::CopyFileToTemporaryDirectory(FString OriginalFileName, FString PathToDirectoryInsideTemporaryFolder, FString TemporaryFileName)
{
    FString pathToTempDirectory = FPaths::ProjectDir() + TEXT("Temp/") + PathToDirectoryInsideTemporaryFolder;

    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.DirectoryExists(*pathToTempDirectory)) {
        if (!FileManager.DeleteDirectoryRecursively(*pathToTempDirectory)) {
            return false;
        }
    }


    if (!FileManager.CreateDirectoryTree(*pathToTempDirectory)) {
        return false;
    }

    FString pathToOriginalFile = FPaths::ProjectSavedDir() + TEXT("Save/") + OriginalFileName;

    if (!FileManager.FileExists(*pathToOriginalFile)) {
        return false;
    }

    FString pathToNewFile = pathToTempDirectory + TEXT("/") + TemporaryFileName;

    if (!FileManager.CopyFile(*pathToNewFile, *pathToOriginalFile)) {
        return false;
    }

    return true;
}
