// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/MyFileManager.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(MyFileManager);

UMyFileManager::UMyFileManager()
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(MyFileManager, Warning, TEXT("Warning in MyFileManager class in constructor - GameInstance was not retrieved from the world"));
}

//Функция, перемещающая указанный файл в указанную директорию внутри временной папки
bool UMyFileManager::CopyFileToTemporaryDirectory(FString PathToOriginalFile, FString PathToDirectoryInsideTempFolder, FString TemporaryFileName)
{
    //Сначала проверяется существует ли оригинальный файл
    if (!FileManager.FileExists(*PathToOriginalFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - File %s does not exist"), *PathToOriginalFile);
        return false;
    }

    FString pathToTempDirectory = FPaths::ProjectDir() + TEXT("Temp/") + PathToDirectoryInsideTempFolder;

    //Если целевая временная директория уже существует, то она удаляется для очистки всего её содержимого
    if (FileManager.DirectoryExists(*pathToTempDirectory)) {
        if (!FileManager.DeleteDirectoryRecursively(*pathToTempDirectory)) {
            UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to delete the temporary directory along the path %s"), *pathToTempDirectory);
            return false;
        }
        else if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The temporary directory %s was removed to clean it up"), *pathToTempDirectory);
    }

    //Затем временная директория вновь создаётся уже пустой
    if (!FileManager.CreateDirectoryTree(*pathToTempDirectory)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to create the temporary directory along the path %s"), *pathToTempDirectory);
        return false;
    }
    else if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The temporary directory %s has been created"), *pathToTempDirectory);

    FString pathToNewFile = pathToTempDirectory + TEXT("/") + TemporaryFileName;

    //Оригинальный файл перемещается под новым именем во временную директорию
    if (!FileManager.CopyFile(*pathToNewFile, *PathToOriginalFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the CopyFileToTemporaryDirectory function - An error occurred while trying to copy the file %s to %s"), *PathToOriginalFile, *pathToNewFile);
        return false;
    }
    else if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the CopyFileToTemporaryDirectory function: The original file %s was moved to a temporary directory, its new path is %s"), *PathToOriginalFile, *pathToNewFile);

    return true;
}

/* Функция, сохраняющая временный файл в другую директорию. При необходимости заменяет файл
     * на место которого сохраняется, старый файл при этом сохраняется в директорию бекапа */
bool UMyFileManager::SaveTempFileToOriginalDirectory(FString PathToOriginalFile, FString PathToTempFile, FString PathToBackUp)
{
    //Сначала проверяетсясуществует ли временный файл, которой необходимо сохранить
    if (!FileManager.FileExists(*PathToTempFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - File %s does not exist"), *PathToTempFile);
        return false;
    }

    FString PathToBackUpFile;
    //Файл, указанный по пути PathToOriginalFile может и не существовать, но если он есть, то от него надо избавиться
    if (FileManager.FileExists(*PathToOriginalFile)) {
        //Просто удалять файл неразумно, его следует переместить в директорию бекапа

        //Сначала создаётся директория бекапов карт, если таковой не было
        if (!FileManager.DirectoryExists(*PathToBackUp)) {
            if (!FileManager.CreateDirectoryTree(*PathToBackUp)) {
                UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred while trying to create the directory %s"), *PathToBackUp);
                return false;
            }
            else if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The backup directory %s has been created"), *PathToBackUp);
        }

        //Затем из пути оригинального файла выделяется одно только название, чтобы присвоить такое же файлу бекапа

        //Находится индекс первого с конца слеша, соответственно всё что будет правее его - название файла
        int LastSlashIndex = PathToOriginalFile.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        //Строка пути до оригинального файла обрезается до индекса слеша включительно, сохраняя только название оригинального файла
        FString OriginalFileName = PathToOriginalFile.RightChop(LastSlashIndex + 1);
        //Используя это название создаётся путь до файла бекапа
        PathToBackUpFile = PathToBackUp + "/" + OriginalFileName;

        //Если данный файл уже сохранялся в бекап, его старая версия удаляется
        if (FileManager.FileExists(*PathToBackUpFile)) {
            if (!FileManager.DeleteFile(*PathToBackUpFile)) {
                UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to delete the backup file %s"), *PathToBackUpFile);
                return false;
            }
            else if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The old version of the backup file %s has been deleted"), *PathToBackUpFile);
        }

        //Затем оригинальный файл переносится в директорию бекапа
        if (!FileManager.MoveFile(*PathToBackUpFile, *PathToOriginalFile)) {
            UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to transfer the file %s to the backup directory %s"), * PathToOriginalFile , *PathToBackUpFile);
            return false;
        }
        else if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(MyFileManager, Log, TEXT("MyFileManager class in the SaveTempFileToOriginalDirectory function: The file %s was moved to the backup directory %s"), *PathToOriginalFile, *PathToBackUpFile);
    }

    //И в конце временный файл переносится на место оригинального
    if (!FileManager.CopyFile(*PathToOriginalFile, *PathToTempFile)) {
        UE_LOG(MyFileManager, Error, TEXT("!!! An error occurred in the MyFileManager class in the SaveTempFileToOriginalDirectory function - An error occurred when trying to move the temporary file %s to the place of the original file %s. The original file will be restored."), *PathToTempFile, *PathToOriginalFile);

        //Если файл перенести не удалось, старый файл восстанавливается из файла бекапа
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
