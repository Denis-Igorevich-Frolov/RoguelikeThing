// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/MyFileManager.h"

DEFINE_LOG_CATEGORY(MyFileManager);

bool UMyFileManager::CopyFileToTemporaryDirectory(FString PathToOriginalFile, FString PathToDirectoryInsideTempFolder, FString TemporaryFileName)
{
    FString pathToTempDirectory = FPaths::ProjectDir() + TEXT("Temp/") + PathToDirectoryInsideTempFolder;

    if (FileManager.DirectoryExists(*pathToTempDirectory)) {
        if (!FileManager.DeleteDirectoryRecursively(*pathToTempDirectory)) {
            return false;
        }
    }


    if (!FileManager.CreateDirectoryTree(*pathToTempDirectory)) {
        return false;
    }

    if (!FileManager.FileExists(*PathToOriginalFile)) {
        return false;
    }

    FString pathToNewFile = pathToTempDirectory + TEXT("/") + TemporaryFileName;

    if (!FileManager.CopyFile(*pathToNewFile, *PathToOriginalFile)) {
        return false;
    }

    return true;
}

bool UMyFileManager::SaveTempFileToOriginalDirectory(FString PathToOriginalFile, FString PathToTempFile)
{
    if (!FileManager.FileExists(*PathToTempFile)) {
        return false;
    }

    if (FileManager.FileExists(*PathToOriginalFile)) {
        FString pathToBackUp = FPaths::ProjectDir() + TEXT("BackUp/MapEditor");
        if (!FileManager.DirectoryExists(*pathToBackUp)) {
            if (!FileManager.CreateDirectoryTree(*pathToBackUp)) {
                return false;
            }
        }

        int LastSlashIndex = PathToOriginalFile.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        FString OriginalFileName = PathToOriginalFile.RightChop(LastSlashIndex + 1);
        FString pathToBackUpFile = pathToBackUp + "/" + OriginalFileName;

        if (FileManager.FileExists(*pathToBackUpFile)) {
            if (!FileManager.DeleteFile(*pathToBackUpFile)) {
                return false;
            }
        }

        if (!FileManager.MoveFile(*pathToBackUpFile, *PathToOriginalFile)) {
            return false;
        }
    }

    if (!FileManager.CopyFile(*PathToOriginalFile, *PathToTempFile)) {
        return false;
    }

    return true;
}
