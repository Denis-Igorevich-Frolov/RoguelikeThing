// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/TexturePreparer.h"
#include "ImageUtils.h"
#include "RoguelikeThing/GameObjects/ObjectsData/FileBinArrayData.h"
#include <RoguelikeThing/SaveGame/DataSaver.h>

DEFINE_LOG_CATEGORY(TexturePreparer);

void UTexturePreparer::PrepareAllExpeditionInteractionObjectsTextures(
    UPARAM(ref)UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, UPARAM(ref)UTextureContainer* ExpeditionInteractionObjectTexturesContainer)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ExpeditionInteractionObjectContainer, ExpeditionInteractionObjectTexturesContainer, this]() {
        if (!ExpeditionInteractionObjectContainer) {
            UE_LOG(TexturePreparer, Error, TEXT("!!!!!!!"));
            return;
        }

        TMap<FString, UExpeditionInteractionObjectData*> InteractionObjects = ExpeditionInteractionObjectContainer->GetAllInteractionObjects();
        TArray<FString> InteractionObjectsKeys;
        InteractionObjects.GenerateKeyArray(InteractionObjectsKeys);

        for (FString ItemName : InteractionObjectsKeys){
            //UE_LOG(TexturePreparer, Error, TEXT("ItemName %s"), *ItemName);

            UPROPERTY()
            UFileBinArrayData* FileBinArrayData = nullptr;
            AsyncTask(ENamedThreads::GameThread, [&FileBinArrayData, this]() {
                FileBinArrayData = NewObject<UFileBinArrayData>();
                });

            while (!FileBinArrayData) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            UExpeditionInteractionObjectData* Data = *InteractionObjects.Find(ItemName);

            TMap<FString, FString> TexturePaths = Data->TexturePaths;
            TArray<FString> Keys;
            TexturePaths.GenerateKeyArray(Keys);

            UPROPERTY()
            UDataSaver* Saver = nullptr;
            //Ѕезопасное создание UObject'ов гарантировано только в основном потоке
            AsyncTask(ENamedThreads::GameThread, [&Saver, this]() {
                Saver = NewObject<UDataSaver>();
                Saver->AddToRoot();
                });

            while (!Saver) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            FString SavFilePath = FPaths::ProjectDir() + Data->ModuleLocalPath + "sav/Textures.sav";

            IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
            bool DataSaverFileExist = FileManager.FileExists(*SavFilePath);

            //≈сли существует сериализованный файл данных о модуле, он загружаетс€
            if (DataSaverFileExist) {
                Saver->LoadBinArray(SavFilePath);
            }

            if (DataSaverFileExist && Saver && Saver->CheckHashChange() && SavFilePath == Saver->GetSavFilePath()) {
                UE_LOG(TexturePreparer, Error, TEXT("Load sav"));

                TMap<FString, UFileBinArrayData*> DataArray = Saver->GetData<UFileBinArrayData>(true);

                TArray<FString> DataPaths;
                DataArray.GenerateKeyArray(DataPaths);

                for (FString DataPath : DataPaths) {

                    TArray<FString> PathPieces;
                    DataPath.ParseIntoArray(PathPieces, TEXT("/"));

                    UFileBinArrayData* FileBinData = *DataArray.Find(DataPath);
                    UPROPERTY()
                    UTexture2D* texture = nullptr;

                    AsyncTask(ENamedThreads::GameThread, [FileBinData, &PathPieces, &texture, &ExpeditionInteractionObjectTexturesContainer, Data, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces[PathPieces.Num() - 4] + "/sav/Textures.sav"), FColor::FromHex("160124"));

                        texture = FImageUtils::ImportBufferAsTexture2D(FileBinData->FileBinary.ObjectsBinArray);
                        });

                    while (!texture) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    ExpeditionInteractionObjectTexturesContainer->AddTexture(PathPieces[PathPieces.Num() - 4], Data->Category,
                        Data->SubCategory, Data->id, FileBinData->FileBinary.FileTag, texture);
                }
            }
            else {
                UE_LOG(TexturePreparer, Error, TEXT("Load png"));
                FString LastLacalFilePath;
                for (FString FileTag : Keys) {

                    FString Value = *TexturePaths.Find(FileTag);
                    LastLacalFilePath = Value;

                    TArray<FString> PathPieces;
                    Value.ParseIntoArray(PathPieces, TEXT("/"));

                    TArray<uint8> FileBinArray;
                    FString FullTextureFilePath;

                    UPROPERTY()
                    UTexture2D* texture = nullptr;
                    AsyncTask(ENamedThreads::GameThread, [&FullTextureFilePath, &FileBinArray, &PathPieces, &texture, &ExpeditionInteractionObjectTexturesContainer, Data, Value, FileTag, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces[PathPieces.Num() - 1]), FColor::FromHex("160124"));

                        FullTextureFilePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + Value);
                        FFileHelper::LoadFileToArray(FileBinArray, *FullTextureFilePath);

                        texture = FImageUtils::ImportBufferAsTexture2D(FileBinArray);
                        });

                    while (!texture) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    UPROPERTY()
                    FFileBinary FileBinary;

                    FileBinary.FileTag = FileTag;
                    FileBinary.ObjectsBinArray = FileBinArray;

                    FileBinArrayData->ItemName = ItemName;
                    FileBinArrayData->FileBinary = FileBinary;

                    Saver->AddDataToBinArray<UFileBinArrayData>(FileBinArrayData, Value, FullTextureFilePath);

                    ExpeditionInteractionObjectTexturesContainer->AddTexture(PathPieces[PathPieces.Num() - 4], Data->Category, Data->SubCategory, Data->id, FileTag, texture);
                }

                TArray<FString> SavPathPieces;
                LastLacalFilePath.ParseIntoArray(SavPathPieces, TEXT("/"));

                FString ModuleSavPath = FPaths::ProjectDir();

                for (int i = 0; i < SavPathPieces.Num() - 3; i++) {
                    ModuleSavPath += SavPathPieces[i];
                    ModuleSavPath += "/";
                }

                ModuleSavPath += "sav/Textures.sav";

                Saver->SaveBinArray(ModuleSavPath);
            }

        }

        AsyncTask(ENamedThreads::GameThread, [this]() {
            ChangeTextOfTheDownloadDetails.Broadcast(FString("Data loading complite"), FColor::Green);
            LoadingComplet.Broadcast();
            });
        });
}