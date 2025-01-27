// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/ExpeditionInteractionObjectsSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <Kismet/GameplayStatics.h>

void UExpeditionInteractionObjectsSaver::AddExpeditionInteractionObjectDataToBinArray(UExpeditionInteractionObjectData* ExpeditionInteractionObjectData, FString FilePath)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    if (FileManager.FileExists(*FilePath)) {
        UPROPERTY(SaveGame)
        FBinArrayWrapper BinArrayWrapper;
        FMemoryWriter InteractionObjectWriter = FMemoryWriter(BinArrayWrapper.BinArray);
        FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectWriter, false);
        InteractionObjectAr.ArIsSaveGame = true;
        ExpeditionInteractionObjectData->Serialize(InteractionObjectAr);

        BinExpeditionInteractionObjectsData.Add(FilePath, BinArrayWrapper);

        FMD5Hash FileHash = FMD5Hash::HashFile(*FilePath);
        FString ExpeditionInteractionObjectHash = LexToString(FileHash);

        XMLFilesHash.Add(FilePath, ExpeditionInteractionObjectHash);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("!!!File not exist"));
    }

}

void UExpeditionInteractionObjectsSaver::SaveBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FMemoryWriter InteractionObjectsWriter = FMemoryWriter(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectsWriter, false);
    InteractionObjectAr.ArIsSaveGame = true;
    this->Serialize(InteractionObjectAr);

    UGameplayStatics::SaveDataToSlot(BinArrayData, FilePath.LeftChop(4), 0);
}

void UExpeditionInteractionObjectsSaver::LoadBinArray(FString FilePath)
{
    TArray<uint8> BinArrayData;
    FFileHelper::LoadFileToArray(BinArrayData, *FilePath);

    FMemoryReader InteractionObjectsReader = FMemoryReader(BinArrayData);
    FObjectAndNameAsStringProxyArchive InteractionObjectsAr(InteractionObjectsReader, false);
    InteractionObjectsAr.ArIsSaveGame = true;

    UExpeditionInteractionObjectsSaver* ExpeditionInteractionObjectsSaver = NewObject<UExpeditionInteractionObjectsSaver>();
    ExpeditionInteractionObjectsSaver->Serialize(InteractionObjectsAr);

    BinExpeditionInteractionObjectsData = ExpeditionInteractionObjectsSaver->BinExpeditionInteractionObjectsData;
    XMLFilesHash = ExpeditionInteractionObjectsSaver->XMLFilesHash;

    ExpeditionInteractionObjectsSaver->MarkPendingKill();
}

bool UExpeditionInteractionObjectsSaver::CheckHashChange()
{
    TArray<FString> XMLFilesPaths;
    XMLFilesHash.GenerateKeyArray(XMLFilesPaths);

    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
    for (FString FilePath : XMLFilesPaths) {
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

int UExpeditionInteractionObjectsSaver::GetBinArraySize()
{
    TArray<FString> Array;
    BinExpeditionInteractionObjectsData.GenerateKeyArray(Array);
    return Array.Num();
}

void UExpeditionInteractionObjectsSaver::ClearArray()
{
    BinExpeditionInteractionObjectsData.Empty();
    XMLFilesHash.Empty();
}

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
            UE_LOG(LogTemp, Error, TEXT("!BinArrayWrapper"));
        }
    }

    return Result;
}
