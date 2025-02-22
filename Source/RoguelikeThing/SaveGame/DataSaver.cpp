// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/DataSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(DataSaver);

/* �������, ����������� ������ � �������� �������������� ���������� � ������ ���� ��������
 * ���������������� ������ ��� ������������ ����������. ����� ����������� ��� ��������� xml ����� */
template<typename Data>
void UDataSaver::AddDataToBinArray(Data* ObjectData, FString SavedLocalFilePath, FString FullFilePath)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.FileExists(*FullFilePath)) {
        //� ������� ������ ������������ ��������������� ������ ���������� ObjectData
        UPROPERTY(SaveGame)
        FBinArrayWrapper BinArrayWrapper;
        FMemoryWriter InteractionObjectWriter = FMemoryWriter(BinArrayWrapper.BinArray);
        FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectWriter, false);
        InteractionObjectAr.ArIsSaveGame = true;
        ObjectData->Serialize(InteractionObjectAr);

        BinData.Add(SavedLocalFilePath, BinArrayWrapper);

        //������������ ��� ��������� xml �����, ������� ����������� ��� ��������� ��������, ����� ��������� ��������� ��������� �����
        FMD5Hash FileHash = FMD5Hash::HashFile(*FullFilePath);
        FString Hash = LexToString(FileHash);

        XMLFilesHash.Add(SavedLocalFilePath, Hash);
    }
    else {
        UE_LOG(DataSaver, Error, TEXT("!!! An error occurred in the DataSaver class in the AddDataToBinArray function - file %s is not exist"), *FullFilePath);
    }
}

/* ��������� �������������� ������ �������� �������������� ����������, ����������� �
 * ������ ������, ����������� � sav ����, ������� ����� ��������� �� ����������� ���� */
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

/* �������� ������� �������� �������������� ����������, ������� ��� ������� � ���� �� ����������� ����.
 * ����� ����� ������� ���� ������, ������� ��������������� �������� GetData */
void UDataSaver::LoadBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FFileHelper::LoadFileToArray(BinArrayData, *FilePath);

    FMemoryReader InteractionObjectsReader = FMemoryReader(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectsAr(InteractionObjectsReader, false);
    InteractionObjectsAr.ArIsSaveGame = true;

    //�������� ������������� ��������� ����� �� ������, ������ ��� ������������ �� this ���-�� �� ���������
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

        //�� �������������� ���������� ��� ��������� �������������� �� �������
        BinData = Saver->BinData;
        XMLFilesHash = Saver->XMLFilesHash;
        SavFilePath = Saver->SavFilePath;

        //� � ����� ������������� ��������� ������������ �� �������������
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

//�������� ��������� ����� xml �����, ��������� � ������� ����������� �������. true - ��������� ���, false - ����
bool UDataSaver::CheckHashChange()
{
    TArray<FString> XMLFilesPaths;
    XMLFilesHash.GenerateKeyArray(XMLFilesPaths);

    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
    for (FString FilePath : XMLFilesPaths) {
        FString FullFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + FilePath;
        //���� �����, ������� ������ ���, ������ ���, �� ������������� ��������� �������� �����������
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

//��������� ���������� ���������, � �������������� � xml ������, �� ��������� ������� ���� ������������ ���������� � ���� ��������� ������
int UDataSaver::GetBinArraySize()
{
    TArray<FString> Array;
    BinData.GenerateKeyArray(Array);
    return Array.Num();
}

//������ ������� ���� �������� ������� ������
void UDataSaver::ClearArray()
{
    BinData.Empty();
    XMLFilesHash.Empty();
}

/* ��������� ��������� ���� �������� �������������� ���������� ������������ ������. ������ ���������
 * �������� ���� �� ��������� xml �����, � ��������� - ��������������� ������ �� ����� ������� */
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
