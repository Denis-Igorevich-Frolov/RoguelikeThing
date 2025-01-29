// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/ExpeditionInteractionObjectsSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(ExpeditionInteractionObjectsSaver);

/* Функция, добавляющая данные о предмете взаимодействия экспедиции в массив всех объектов
 * соответствующего модуля для последующего сохранения. Также сохраняется хеш исходного xml файла */
void UExpeditionInteractionObjectsSaver::AddExpeditionInteractionObjectDataToBinArray(UExpeditionInteractionObjectData* ExpeditionInteractionObjectData, FString FilePath)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.FileExists(*FilePath)) {
        //В бинрную обёртку записываются сериализованные данные переменной ExpeditionInteractionObjectData
        UPROPERTY(SaveGame)
        FBinArrayWrapper BinArrayWrapper;
        FMemoryWriter InteractionObjectWriter = FMemoryWriter(BinArrayWrapper.BinArray);
        FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectWriter, false);
        InteractionObjectAr.ArIsSaveGame = true;
        ExpeditionInteractionObjectData->Serialize(InteractionObjectAr);

        BinExpeditionInteractionObjectsData.Add(FilePath, BinArrayWrapper);

        //Запоминается хеш исходного xml файла, который понадобится при следующей загрузке, чтобы отследить возможное изменение файла
        FMD5Hash FileHash = FMD5Hash::HashFile(*FilePath);
        FString ExpeditionInteractionObjectHash = LexToString(FileHash);

        XMLFilesHash.Add(FilePath, ExpeditionInteractionObjectHash);
    }
    else {
        UE_LOG(ExpeditionInteractionObjectsSaver, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectsSaver class in the AddExpeditionInteractionObjectDataToBinArray function - file %s is not exist"), *FilePath);
    }

}

/* Полностью сформированный массив объектов взаимодействия экспедиции, относящихся к
 * одному модулю, сохраняется в sav файл, который будет находится по переданному пути */
void UExpeditionInteractionObjectsSaver::SaveBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FMemoryWriter InteractionObjectsWriter = FMemoryWriter(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectsWriter, false);
    InteractionObjectAr.ArIsSaveGame = true;
    this->Serialize(InteractionObjectAr);

    UGameplayStatics::SaveDataToSlot(BinArrayData, FilePath.LeftChop(4), 0);
}

/* Загрузка массива объектов взаимодействия экспедиции, который был сохранён в слот по переданному пути.
 * Чтобы затем извлечь этот массив, следует воспользоваться функцией GetExpeditionInteractionObjectsData */
void UExpeditionInteractionObjectsSaver::LoadBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FFileHelper::LoadFileToArray(BinArrayData, *FilePath);

    FMemoryReader InteractionObjectsReader = FMemoryReader(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectsAr(InteractionObjectsReader, false);
    InteractionObjectsAr.ArIsSaveGame = true;

    //Создаётся промежуточный экземпляр этого же класса, потому что сериализация на this как-то не сработала
    UExpeditionInteractionObjectsSaver* Saver = NewObject<UExpeditionInteractionObjectsSaver>();

    if (Saver) {
        Saver->Serialize(InteractionObjectsAr);

        //Из промежуточного экземпляра все коллекции перекидываются на текущий
        BinExpeditionInteractionObjectsData = Saver->BinExpeditionInteractionObjectsData;
        XMLFilesHash = Saver->XMLFilesHash;

        //И в конце промежуточный экземпляр уничтожается за ненадобностью
        if (Saver->IsValidLowLevel())
            Saver->MarkPendingKill();
    }
    else {
        UE_LOG(ExpeditionInteractionObjectsSaver, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectsSaver class in the LoadBinArray function - ExpeditionInteractionObjectsSaver is not valid"));
    }
}

//Проверка изменения хешей xml фалов, связанных с текущим загруженным модулем. true - изменений нет, false - есть
bool UExpeditionInteractionObjectsSaver::CheckHashChange()
{
    TArray<FString> XMLFilesPaths;
    XMLFilesHash.GenerateKeyArray(XMLFilesPaths);

    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
    for (FString FilePath : XMLFilesPaths) {
        //Если файла, который раньше был, больше нет, то следовательно изменения очевидно происходили
        if (FileManager.FileExists(*FilePath)) {
            FString OldFileHash = *XMLFilesHash.Find(FilePath);

            FMD5Hash FileHash = FMD5Hash::HashFile(*FilePath);
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
int UExpeditionInteractionObjectsSaver::GetBinArraySize()
{
    TArray<FString> Array;
    BinExpeditionInteractionObjectsData.GenerateKeyArray(Array);
    return Array.Num();
}

//Полная очистка всех массивов данного модуля
void UExpeditionInteractionObjectsSaver::ClearArray()
{
    BinExpeditionInteractionObjectsData.Empty();
    XMLFilesHash.Empty();
}

/* Получение коллекции всех объектов взаимодействия экспедиции загруженного модуля. Ключом коллекции
 * является путь до исходного xml файла, а значением - непосредственно данные по этому объекту */
TMap<FString, UExpeditionInteractionObjectData*> UExpeditionInteractionObjectsSaver::GetExpeditionInteractionObjectsData()
{
    TMap<FString, UExpeditionInteractionObjectData*> Result;

    TArray<FString> BinExpeditionInteractionObjectsDataKeys;
    BinExpeditionInteractionObjectsData.GenerateKeyArray(BinExpeditionInteractionObjectsDataKeys);

    for (FString Key : BinExpeditionInteractionObjectsDataKeys) {
        FBinArrayWrapper* BinArrayWrapper = BinExpeditionInteractionObjectsData.Find(Key);

        if (BinArrayWrapper) {
            FMemoryReader InteractionObjectReader = FMemoryReader(BinArrayWrapper->BinArray);
            FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectReader, false);
            InteractionObjectAr.ArIsSaveGame = true;

            UPROPERTY()
            UExpeditionInteractionObjectData* ExpeditionInteractionObjectData = NewObject<UExpeditionInteractionObjectData>();
            ExpeditionInteractionObjectData->Serialize(InteractionObjectAr);

            Result.Add(Key, ExpeditionInteractionObjectData);
        }
        else {
            UE_LOG(ExpeditionInteractionObjectsSaver, Error, TEXT("!!! An error occurred in the ExpeditionInteractionObjectsSaver class in the GetExpeditionInteractionObjectsData function - BinArrayWrapper of object %s is not valid"), *Key);
        }
    }

    return Result;
}
