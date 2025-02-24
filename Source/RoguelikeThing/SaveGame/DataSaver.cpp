// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/DataSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(DataSaver);

/* Функция, добавляющая данные о объекте в массив всех объектов соответствующего
 * модуля для последующего сохранения. Также сохраняется хеш исходного xml файла */
template<typename Data>
void UDataSaver::AddDataToBinArray(Data* ObjectData, FString SavedLocalFilePath, FString FullFilePath)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.FileExists(*FullFilePath)) {
        //В бинрную обёртку записываются сериализованные данные переменной ObjectData
        UPROPERTY(SaveGame)
        FBinArrayWrapper BinArrayWrapper;
        FMemoryWriter ObjectWriter = FMemoryWriter(BinArrayWrapper.BinArray);
        FObjectAndNameAsStringProxyArchive ObjectAr(ObjectWriter, false);
        ObjectAr.ArIsSaveGame = true;
        ObjectData->Serialize(ObjectAr);

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

/* Полностью сформированный массив объектов относящихся к сохраняемому модулю,
 * записывается в sav файл, который будет находится по переданному пути */
void UDataSaver::SaveBinArray(FString FilePath)
{
    SavFilePath = FilePath;
    TArray<uint8> BinArrayData;
    FMemoryWriter ObjectsWriter = FMemoryWriter(BinArrayData);
    FObjectAndNameAsStringProxyArchive ObjectAr(ObjectsWriter, false);
    ObjectAr.ArIsSaveGame = true;
    this->Serialize(ObjectAr);

    if (!UGameplayStatics::SaveDataToSlot(BinArrayData, FilePath.LeftChop(4), 0)) {
        UE_LOG(DataSaver, Error, TEXT("!!! An error occurred in the DataSaver class in the SaveBinArray function - Failed to save data to slot %s"), *FilePath);
    }
}

/* Загрузка массива объектов модуля, который был сохранён в слот по переданному пути.
 * Чтобы затем извлечь этот массив, следует воспользоваться функцией GetData */
void UDataSaver::LoadBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FFileHelper::LoadFileToArray(BinArrayData, *FilePath);

    FMemoryReader ObjectsReader = FMemoryReader(BinArrayData);
    FObjectAndNameAsStringProxyArchive ObjectsAr(ObjectsReader, false);

    //Создаётся промежуточный экземпляр этого же класса, потому что сериализация на this как-то не сработала
    UPROPERTY()
    UDataSaver* Saver = nullptr;

    //Безопасное создание UObject'ов гарантировано только в основном потоке
    AsyncTask(ENamedThreads::GameThread, [&Saver, this]() {
        Saver = NewObject<UDataSaver>();
        /* Так как изначально Saver равен nullptr, сборщик мусора то и дело наравит вмешаться
         * в жизненный цикл переменной, так что её следует зарутировать от него подальше */
        Saver->AddToRoot();
        });

    //А пока Saver инициализируется в основном потоке, асинхронный ждёт, пока переменная не будет готова
    while (!Saver) {
        FPlatformProcess::SleepNoStats(0.0f);
    }

    if (Saver) {
        Saver->Serialize(ObjectsAr);

        //Из промежуточного экземпляра все данные перекидываются на текущий
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
        if (FileManager.FileExists(*FullFilePath)) {
            FString OldFileHash = *XMLFilesHash.Find(FilePath);

            FMD5Hash FileHash = FMD5Hash::HashFile(*FullFilePath);
            FString CurrentFileHas = LexToString(FileHash);

            if (CurrentFileHas != OldFileHash) {
                return false;
            }
        }
        //Если файла, который раньше был, больше нет, то следовательно изменения очевидно происходили
        else {
            return false;
        }
    }

    return true;
}

//Получение количества объектов, а соответственно и xml файлов, на основании которых была сформирована информация о всех объктах модуля
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

/* Получение коллекции всех объектов загружаемого модуля. Ключом коллекции является
 * путь до исходного xml файла, а значением - непосредственно данные этого объекта.
 * Из-за создания экземпляров UObject эту функцию безопасно запускать только в основном потоке */
template<typename Data>
TMap<FString, Data*> UDataSaver::GetData()
{
    TMap<FString, Data*> Result;

    TArray<FString> BinDataKeys;
    BinData.GenerateKeyArray(BinDataKeys);

    for (FString Key : BinDataKeys) {
        FBinArrayWrapper* BinArrayWrapper = BinData.Find(Key);

        if (BinArrayWrapper) {
            FMemoryReader ObjectReader = FMemoryReader(BinArrayWrapper->BinArray);
            FObjectAndNameAsStringProxyArchive ObjectAr(ObjectReader, false);

            UPROPERTY()
            Data* ObjectData = NewObject<Data>();

            ObjectData->Serialize(ObjectAr);

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
