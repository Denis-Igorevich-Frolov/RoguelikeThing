// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/TexturePreparer.h"
#include "ImageUtils.h"
#include "RoguelikeThing/GameObjects/ObjectsData/FileBinArrayData.h"
#include <RoguelikeThing/SaveGame/DataSaver.h>

DEFINE_LOG_CATEGORY(TexturePreparer);

template<typename GameObjectContainer, typename GameObjectData>
void UTexturePreparer::PrepareAllTextures(GameObjectContainer* ObjectContainer, UTextureContainer* TexturesContainer)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ObjectContainer, TexturesContainer, this]() {
        if (!ObjectContainer) {
            UE_LOG(TexturePreparer, Error, TEXT("!!!!!!!"));
            return;
        }

        TArray<FString> ModulesName = ObjectContainer->GetAllModulesName();
        for (FString ModuleName : ModulesName) {
            TMap<FString, GameObjectData*> InteractionObjects = ObjectContainer->GetAllObjectsFromeModule(ModuleName);
            TArray<FString> InteractionObjectsKeys;
            InteractionObjects.GenerateKeyArray(InteractionObjectsKeys);

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

            if (InteractionObjectsKeys.Num() == 0) {
                UE_LOG(TexturePreparer, Error, TEXT("!!! InteractionObjectsKeys.Num() == 0"));
                return;
            }

            GameObjectData* TestData = *InteractionObjects.Find(InteractionObjectsKeys[0]);

            FString SavFilePath = FPaths::ProjectDir() + TestData->ModuleLocalPath + "sav/Textures.sav";

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

                    AsyncTask(ENamedThreads::GameThread, [FileBinData, &PathPieces, &texture, &TexturesContainer, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces[PathPieces.Num() - 4] + "/sav/Textures.sav"), FColor::FromHex("160124"));

                        texture = FImageUtils::ImportBufferAsTexture2D(FileBinData->FileBinary.ObjectsBinArray);
                        });

                    while (!texture) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    TexturesContainer->AddTexture(FileBinData->ModuleName, FileBinData->CategoryName,
                        FileBinData->SubCategoryName, FileBinData->ItemName, FileBinData->TextureTag, texture);
                }
            }
            else {
                FString FirstLacalFilePath;

                for (FString ItemName : InteractionObjectsKeys) {
                    UE_LOG(TexturePreparer, Error, TEXT("Load png"));

                    UPROPERTY()
                    UFileBinArrayData* FileBinArrayData = nullptr;
                    AsyncTask(ENamedThreads::GameThread, [&FileBinArrayData, this]() {
                        FileBinArrayData = NewObject<UFileBinArrayData>();
                        });

                    while (!FileBinArrayData) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    GameObjectData* Data = *InteractionObjects.Find(ItemName);

                    TMap<FString, FString> TexturePaths = Data->TexturePaths;
                    TArray<FString> Keys;
                    TexturePaths.GenerateKeyArray(Keys);

                    for (FString FileTag : Keys) {

                        FString Value = *TexturePaths.Find(FileTag);
                        if(FirstLacalFilePath.IsEmpty())
                            FirstLacalFilePath = Value;

                        TArray<FString> PathPieces;
                        Value.ParseIntoArray(PathPieces, TEXT("/"));

                        TArray<uint8> FileBinArray;
                        FString FullTextureFilePath;

                        UPROPERTY()
                        UTexture2D* texture = nullptr;
                        AsyncTask(ENamedThreads::GameThread, [&FullTextureFilePath, &FileBinArray, &PathPieces, &texture, &TexturesContainer, Data, Value, FileTag, this]() {
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

                        FileBinary.ObjectsBinArray = FileBinArray;

                        FileBinArrayData->Init(PathPieces[PathPieces.Num() - 4], Data->Category, Data->SubCategory, Data->id, FileTag, FileBinary);

                        Saver->AddDataToBinArray<UFileBinArrayData>(FileBinArrayData, Value, FullTextureFilePath);

                        TexturesContainer->AddTexture(PathPieces[PathPieces.Num() - 4], Data->Category, Data->SubCategory, Data->id, FileTag, texture);
                    }
                }

                TArray<FString> SavPathPieces;
                FirstLacalFilePath.ParseIntoArray(SavPathPieces, TEXT("/"));

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

void UTexturePreparer::PrepareAllExpeditionInteractionObjectsTextures(
    UPARAM(ref)UExpeditionInteractionObjectContainer* ObjectContainer, UPARAM(ref)UTextureContainer* TexturesContainer)
{
    PrepareAllTextures<UExpeditionInteractionObjectContainer, UExpeditionInteractionObjectData>(ObjectContainer, TexturesContainer);
}

void UTexturePreparer::PrepareAllInventoryItemsTextures(UPARAM(ref)UInventoryItemsContainer* InventoryItemContainer, UPARAM(ref)UTextureContainer* InventoryItemTexturesContainer)
{
    PrepareAllTextures<UInventoryItemsContainer, UInventoryItemData>(InventoryItemContainer, InventoryItemTexturesContainer);
}
