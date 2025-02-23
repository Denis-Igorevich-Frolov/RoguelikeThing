// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/ModsPreparer.h"
#include <HAL/FileManagerGeneric.h>

TArray<FString> UModsPreparer::GetArrayOfModDirectories(FString CategoryName)
{
    TArray<FString> Result;

    IFileManager& FileManager = FFileManagerGeneric::Get();

    FString ModsDir = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Mods"));

    if (!FileManager.DirectoryExists(*ModsDir)) {
        FileManager.MakeDirectory(*ModsDir);
    }

    TArray<FString> Dirs;
    FileManager.FindFilesRecursive(Dirs, *ModsDir, TEXT("*"), false, true);

    for (FString Dir : Dirs) {
        if (FileManager.DirectoryExists(*FString(Dir + "/" + CategoryName))) {
            //Получение списка папок всех модулей
            TArray<FString> ModDirs;
            FString StartDirPath = FString(*FString(Dir + "/" + CategoryName));
            FFileManagerGeneric::Get().FindFilesRecursive(ModDirs, *StartDirPath, TEXT("*"), false, true);

            TArray<FString> ModuleNames;
            //Так как функция FindFilesRecursive находит все вложенные директории, а не только корневые, все остальные стоит отсеять
            for (FString ModuleDir : ModDirs) {
                //Путь до директории разбивается на массив последовательно вложенных папок
                TArray<FString> DirPieces;
                ModuleDir.ParseIntoArray(DirPieces, TEXT("/"), false);

                //И проверяется на каком месте находится корень для всех модулей объектов взаимодействия
                int SequentialDirectoryNumber = 0;
                DirPieces.Find(CategoryName, SequentialDirectoryNumber);

                /* Отсееваются некорневые директории по длине пути. Если директория корневая,
                 * то её длинна должна быть равна последовательному номеру корня общего для всех
                 * модулей + 1. Также стоит учеть, что SequentialDirectoryNumber - это индекс,
                 * которому до последовательного номера следует прибавить ещё 1 */
                if (DirPieces.Num() == SequentialDirectoryNumber + 2 && DirPieces.IsValidIndex(SequentialDirectoryNumber + 1)) {
                    ModuleNames.Add(*FString(DirPieces[SequentialDirectoryNumber - 1] + "/" + DirPieces[SequentialDirectoryNumber] + "/" + DirPieces[SequentialDirectoryNumber + 1]));
                }
            }

            Result.Append(ModuleNames);
        }
    }

    return Result;
}

TArray<FString> UModsPreparer::GetArrayOfModDirectoriesHavingExpeditionInteractionObjects()
{
    return GetArrayOfModDirectories("Expedition interaction objects");
}

TArray<FString> UModsPreparer::GetArrayOfModDirectoriesHavingInventoryItems()
{
    return GetArrayOfModDirectories("Inventory items");
}
