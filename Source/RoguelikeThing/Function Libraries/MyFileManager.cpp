// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/MyFileManager.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(MyFileManager);

UMyFileManager::UMyFileManager()
{
    //��������� GameInstance �� ����
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(MyFileManager, Warning, TEXT("Warning in MyFileManager class in constructor - GameInstance was not retrieved from the world"));
}

//�������, ������������ ��������� ���� � ��������� ���������� ������ ��������� �����
bool UMyFileManager::CopyFileToTemporaryDirectory(FString PathToOriginalFile, FString PathToDirectoryInsideTempFolder, FString TemporaryFileName)
{
    //������� ����������� ���������� �� ������������ ����
    if (!FileManager.FileExists(*PathToOriginalFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - File %s does not exist"), *PathToOriginalFile);
        return false;
    }

    FString pathToTempDirectory = FPaths::ProjectDir() + TEXT("Temp/") + PathToDirectoryInsideTempFolder;

    //���� ������� ��������� ���������� ��� ����������, �� ��� ��������� ��� ������� ����� � �����������
    if (FileManager.DirectoryExists(*pathToTempDirectory)) {
        if (!FileManager.DeleteDirectoryRecursively(*pathToTempDirectory)) {
            UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to delete the temporary directory along the path %s"), *pathToTempDirectory);
            return false;
        }
        else if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The temporary directory %s was removed to clean it up"), *pathToTempDirectory);
    }

    //����� ��������� ���������� ����� �������� ��� ������
    if (!FileManager.CreateDirectoryTree(*pathToTempDirectory)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to create the temporary directory along the path %s"), *pathToTempDirectory);
        return false;
    }
    else if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The temporary directory %s has been created"), *pathToTempDirectory);

    FString pathToNewFile = pathToTempDirectory + TEXT("/") + TemporaryFileName;

    //������������ ���� ������������ ��� ����� ������ �� ��������� ����������
    if (!FileManager.CopyFile(*pathToNewFile, *PathToOriginalFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to copy the file %s to %s"), *PathToOriginalFile, *pathToNewFile);
        return false;
    }
    else if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The original file %s was moved to a temporary directory, its new path is %s"), *PathToOriginalFile, *pathToNewFile);

    return true;
}

/* �������, ����������� ��������� ���� � ������ ����������. ��� ������������� �������� ����
     * �� ����� �������� �����������, ������ ���� ��� ���� ����������� � ���������� ������ */
bool UMyFileManager::SaveTempFileToOriginalDirectory(FString PathToOriginalFile, FString PathToTempFile, FString PathToBackUp)
{
    //������� ��������������������� �� ��������� ����, ������� ���������� ���������
    if (!FileManager.FileExists(*PathToTempFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - File %s does not exist"), *PathToTempFile);
        return false;
    }

    FString PathToBackUpFile;
    //����, ��������� �� ���� PathToOriginalFile ����� � �� ������������, �� ���� �� ����, �� �� ���� ���� ����������
    if (FileManager.FileExists(*PathToOriginalFile)) {
        //������ ������� ���� ���������, ��� ������� ����������� � ���������� ������

        //������� �������� ���������� ������� ����, ���� ������� �� ����
        if (!FileManager.DirectoryExists(*PathToBackUp)) {
            if (!FileManager.CreateDirectoryTree(*PathToBackUp)) {
                UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred while trying to create the directory %s"), *PathToBackUp);
                return false;
            }
            else if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The backup directory %s has been created"), *PathToBackUp);
        }

        //����� �� ���� ������������� ����� ���������� ���� ������ ��������, ����� ��������� ����� �� ����� ������

        //��������� ������ ������� � ����� �����, �������������� �� ��� ����� ������ ��� - �������� �����
        int LastSlashIndex = PathToOriginalFile.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        //������ ���� �� ������������� ����� ���������� �� ������� ����� ������������, �������� ������ �������� ������������� �����
        FString OriginalFileName = PathToOriginalFile.RightChop(LastSlashIndex + 1);
        //��������� ��� �������� �������� ���� �� ����� ������
        PathToBackUpFile = PathToBackUp + "/" + OriginalFileName;

        //���� ������ ���� ��� ���������� � �����, ��� ������ ������ ���������
        if (FileManager.FileExists(*PathToBackUpFile)) {
            if (!FileManager.DeleteFile(*PathToBackUpFile)) {
                UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to delete the backup file %s"), *PathToBackUpFile);
                return false;
            }
            else if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The old version of the backup file %s has been deleted"), *PathToBackUpFile);
        }

        //����� ������������ ���� ����������� � ���������� ������
        if (!FileManager.MoveFile(*PathToBackUpFile, *PathToOriginalFile)) {
            UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to transfer the file %s to the backup directory %s"), * PathToOriginalFile , *PathToBackUpFile);
            return false;
        }
        else if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The file %s was moved to the backup directory %s"), *PathToOriginalFile, *PathToBackUpFile);
    }

    //� � ����� ��������� ���� ����������� �� ����� �������������
    if (!FileManager.CopyFile(*PathToOriginalFile, *PathToTempFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to move the temporary file %s to the place of the original file %s. The original file will be restored."), *PathToTempFile, *PathToOriginalFile);

        //���� ���� ��������� �� �������, ������ ���� ����������������� �� ����� ������
        if (FileManager.FileExists(*PathToBackUpFile)) {
            if (FileManager.MoveFile(*PathToOriginalFile, *PathToBackUpFile)) {
                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The backup file %s restored the original file %s"), *PathToBackUpFile, *PathToOriginalFile);
            } else
                UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - When trying to transfer the backup file %s to the place of the lost original file %s an error occurred and automatic file recovery failed. Don't despair, all is not lost yet! The system has determined that a backup file exists on the path %s, you can try to restore it manually by transferring it to the specified path %s"), *PathToBackUpFile, *PathToOriginalFile, *PathToBackUpFile, *PathToOriginalFile);
        }
        else
            UE_LOG(MyFileManager, Warning, TEXT("Warning in MyFileManager class in SaveTempFileToOriginalDirectory function - There is no backup of the original file %s, file recovery is impossible!"), *PathToBackUpFile);

        return false;
    }

    return true;
}
