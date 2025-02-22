// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/DataSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(DataSaver);

/* Функция, добавляющая данные о предмете взаимодействия экспедиции в массив всех объектов
 * соответствующего модуля для последующего сохранения. Также сохраняется хеш исходного xml файла */
template<typename Data>
void UDataSaver::AddDataToBinArray(Data* ObjectData, FString SavedLocalFilePath, FString FullFilePath)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.FileExists(*FullFilePath)) {
        //В бинрную обёртку записываются сериализованные данные переменной ObjectData
        UPROPERTY(SaveGame)
        FBinArrayWrapper BinArrayWrapper;
        FMemoryWriter InteractionObjectWriter = FMemoryWriter(BinArrayWrapper.BinArray);
        FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectWriter, false);
        InteractionObjectAr.ArIsSaveGame = true;
        ObjectData->Serialize(InteractionObjectAr);

        BinData.Add(SavedLocalFilePath, BinArrayWrapper);

        //Запоминается хеш исходного xml файла, который понадобится при следующей загрузке, чтобы отследить возможное изменение файла
        FMD5Hash FileHash = FMD5Hash::HashFile(*FullFilePath);
        FString Hash = LexToString(FileHash);

        XMLFilesHash.Add(SavedLocalFilePath, Hash);
    }
    else {
        UE_LOG(DataSaver, Error, TEXT("!!! An error occurred in the DataSaver class in the AddDataToBinArray function - file %s is not exist"), *FullFilePath);
    }
}

/* Полностью сформированный массив объектов взаимодействия экспедиции, относящихся к
 * одному модулю, сохраняется в sav файл, который будет находится по переданному пути */
void UDataSaver::SaveBinArray(FString FilePath)
{
    SavFilePath = FilePath;
    TArray<uint8> BinArrayData;
    FMemoryWriter InteractionObjectsWriter = FMemoryWriter(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectsWriter, false);
    InteractionObjectAr.ArIsSaveGame = true;
    this->Serialize(InteractionObjectAr);

    if (!UGameplayStatics::SaveDataToSlot(BinArrayData, FilePath.LeftChop(4), 0)) {
        UE_LOG(DataSaver, Error, TEXT("!!! AAAAAAAAAAAAA %s BinArrayData %d"), *FilePath, BinArrayData.Num());
    }
}

/* Загрузка массива объектов взаимодействия экспедиции, который был сохранён в слот по переданному пути.
 * Чтобы затем извлечь этот массив, следует воспользоваться функцией GetData */
void UDataSaver::LoadBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FFileHelper::LoadFileToArray(BinArrayData, *FilePath);

    FMemoryReader InteractionObjectsReader = FMemoryReader(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectsAr(InteractionObjectsReader, false);
    InteractionObjectsAr.ArIsSaveGame = true;

    //Создаётся промежуточный экземпляр этого же класса, потому что сериализация на this как-то не сработала
    UPROPERTY()
    UDataSaver* Saver = nullptr;

    AsyncTask(ENamedThreads::GameThread, [&Saver, this]() {
        Saver = NewObject<UDataSaver>();
        Saver->AddToRoot();
        });

    while (!Saver) {
        FPlatformProcess::SleepNoStats(0.0f);
    }

    if (Saver) {
        Saver->Serialize(InteractionObjectsAr);

        //Из промежуточного экземпляра все коллекции перекидываются на текущий
        BinData = Saver->BinData;
        XMLFilesHash = Saver->XMLFilesHash;
        SavFilePath = Saver->SavFilePath;

        //И в конце промежуточный экземпляр уничтожается за ненадобностью
        if (Saver->IsValidLowLevel()) {
            if (Saver->IsRooted())
                Saver->RemoveFromRoot();

            Saver->MarkPendingKill();
        }
    }
    else {
        UE_LOG(DataSaver, Error, TEXT("!!! An error occurred in the DataSaver class in the LoadBinArray function - Saver is not valid"));
    }
}

//Проверка изменения хешей xml фалов, связанных с текущим загруженным модулем. true - изменений нет, false - есть
bool UDataSaver::CheckHashChange()
{
    TArray<FString> XMLFilesPaths;
    XMLFilesHash.GenerateKeyArray(XMLFilesPaths);

    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
    for (FString FilePath : XMLFilesPaths) {
        FString FullFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FilePath;
        //Если файла, который раньше был, больше нет, то следовательно изменения очевидно происходили
        if (FileManager.FileExists(*FullFilePath)) {
            FString OldFileHash = *XMLFilesHash.Find(FilePath);

            FMD5Hash FileHash = FMD5Hash::HashFile(*FullFilePath);
            FString CurrentFileHas = LexToString(FileHash);

            if (CurrentFileHas != OldFileHash) {
                return false;
            }
        }
        else {
            return false;
        }
    }

    return true;
}

//Получение количества предметов, а соответственно и xml файлов, на основании которых была сформирована информация о всех предметах модуля
int UDataSaver::GetBinArraySize()
{
    TArray<FString> Array;
    BinData.GenerateKeyArray(Array);
    return Array.Num();
}

//Полная очистка всех массивов данного модуля
void UDataSaver::ClearArray()
{
    BinData.Empty();
    XMLFilesHash.Empty();
}

/* Получение коллекции всех объектов взаимодействия экспедиции загруженного модуля. Ключом коллекции
 * является путь до исходного xml файла, а значением - непосредственно данные по этому объекту */
template<typename Data>
TMap<FString, Data*> UDataSaver::GetData()
{
    TMap<FString, Data*> Result;

    TArray<FString> BinDataKeys;
    BinData.GenerateKeyArray(BinDataKeys);

    for (FString Key : BinDataKeys) {
        FBinArrayWrapper* BinArrayWrapper = BinData.Find(Key);

        if (BinArrayWrapper) {
            FMemoryReader InteractionObjectReader = FMemoryReader(BinArrayWrapper->BinArray);
            FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectReader, false);
            InteractionObjectAr.ArIsSaveGame = true;

            UPROPERTY()
            Data* ObjectData = NewObject<Data>();

            ObjectData->Serialize(InteractionObjectAr);

            Result.Add(Key, ObjectData);
        }
        else {
            UE_LOG(DataSaver, Error, TEXT("!!! An error occurred in the DataSaver class in the GetData function - BinArrayWrapper of object %s is not valid"), *Key);
        }
    }

    return Result;
}

const FString UDataSaver::GetSavFilePath()
{
    return SavFilePath;
}
