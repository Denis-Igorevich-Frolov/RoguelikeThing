// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/SoundPreparer.h"

DEFINE_LOG_CATEGORY(SoundPreparer);

template<typename GameObjectContainer, typename GameObjectData>
void USoundPreparer::PrepareAllSounds(GameObjectContainer* ObjectContainer, USoundContainer* SoundsContainer)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ObjectContainer, SoundsContainer, this]() {
        if (!ObjectContainer) {
            UE_LOG(SoundPreparer, Error, TEXT("!!!!!!!"));
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
                UE_LOG(SoundPreparer, Error, TEXT("!!! InteractionObjectsKeys.Num() == 0"));
                return;
            }

            GameObjectData* TestData = *InteractionObjects.Find(InteractionObjectsKeys[0]);

            FString SavFilePath = FPaths::ProjectDir() + TestData->ModuleLocalPath + "sav/Sounds.sav";

            IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
            bool DataSaverFileExist = FileManager.FileExists(*SavFilePath);

            //≈сли существует сериализованный файл данных о модуле, он загружаетс€
            if (DataSaverFileExist) {
                Saver->LoadBinArray(SavFilePath);
            }

            if (DataSaverFileExist && Saver && Saver->CheckHashChange() && SavFilePath == Saver->GetSavFilePath()) {
                UE_LOG(SoundPreparer, Error, TEXT("Load sav"));

                TMap<FString, USoundData*> DataArray = Saver->GetData<USoundData>(true);

                TArray<FString> DataPaths;
                DataArray.GenerateKeyArray(DataPaths);

                for (FString DataPath : DataPaths) {
                    TArray<FString> PathPieces;
                    DataPath.ParseIntoArray(PathPieces, TEXT("/"));

                    USoundData* SoundData = *DataArray.Find(DataPath);
                    UPROPERTY()
                    USoundWave* Sound = nullptr;

                    AsyncTask(ENamedThreads::GameThread, [&PathPieces, &Sound, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces[PathPieces.Num() - 4] + "/sav/Sounds.sav"), FColor::FromHex("160124"));

                        Sound = NewObject<USoundWave>(USoundWave::StaticClass());
                        });

                    while (!Sound) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    Sound->NumChannels = SoundData->NumChannels;
                    Sound->SetSampleRate(SoundData->SampleRate);
                    Sound->Duration = (float)(SoundData->DataSize) / (SoundData->SampleRate * SoundData->NumChannels * (SoundData->BitsPerSample / 8));
                    Sound->RawPCMDataSize = SoundData->DataSize;

                    Sound->RawPCMData = (uint8*)FMemory::Malloc(SoundData->DataSize);
                    FMemory::Memcpy(Sound->RawPCMData, SoundData->PCMData.GetData(), Sound->RawPCMDataSize);

                    //const uint8* PCMData = &SoundData->PCMData;
                    //FMemory::Memcpy(Sound->RawPCMData, PCMData, SoundData->DataSize);

                    Sound->bProcedural = false;
                    Sound->SoundGroup = SOUNDGROUP_Default;
                    Sound->DecompressionType = DTYPE_Setup;

                    SoundsContainer->AddSound(SoundData->ModuleName, SoundData->CategoryName,
                        SoundData->SubCategoryName, SoundData->ItemName, SoundData->SoundTag, Sound);
                }
            }
            else {
                FString FirstLacalFilePath;

                for (FString ItemName : InteractionObjectsKeys) {
                    UE_LOG(SoundPreparer, Error, TEXT("Load wav"));

                    UPROPERTY()
                    USoundData* SoundData = nullptr;
                    AsyncTask(ENamedThreads::GameThread, [&SoundData, this]() {
                        SoundData = NewObject<USoundData>();
                        });

                    while (!SoundData) {
                        FPlatformProcess::SleepNoStats(0.0f);
                    }

                    GameObjectData* Data = *InteractionObjects.Find(ItemName);

                    TMap<FString, FString> SoundPaths = Data->SoundPaths;
                    TArray<FString> Keys;
                    SoundPaths.GenerateKeyArray(Keys);

                    for (FString FileTag : Keys) {

                        FString Value = *SoundPaths.Find(FileTag);
                        if (FirstLacalFilePath.IsEmpty())
                            FirstLacalFilePath = Value;

                        TArray<FString> PathPieces;
                        Value.ParseIntoArray(PathPieces, TEXT("/"));

                        TArray<uint8> FileBinArray;
                        FString FullSoundFilePath;

                        UPROPERTY()
                        USoundWave* Sound = nullptr;
                        AsyncTask(ENamedThreads::GameThread, [&PathPieces, &Sound, this]() {
                            ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces[PathPieces.Num() - 1]), FColor::FromHex("160124"));
                            Sound = NewObject<USoundWave>(USoundWave::StaticClass());
                            });

                        while (!Sound) {
                            FPlatformProcess::SleepNoStats(0.0f);
                        }

                        FullSoundFilePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + Value);

                        TArray<uint8> RawFileData;
                        if (!FFileHelper::LoadFileToArray(RawFileData, *FullSoundFilePath))
                        {
                            UE_LOG(LogTemp, Error, TEXT("Could not load WAV file from path: %s"), *FullSoundFilePath);
                            return;
                        }

                        FWaveModInfo WaveInfo;
                        if (!WaveInfo.ReadWaveInfo(RawFileData.GetData(), RawFileData.Num()))
                        {
                            UE_LOG(LogTemp, Error, TEXT("Invalid WAV file: %s"), *FullSoundFilePath);
                            return;
                        }

                        // ѕолучаем основные параметры
                        uint16 NumChannels = *WaveInfo.pChannels;
                        uint32 SampleRate = *WaveInfo.pSamplesPerSec;
                        uint16 BitsPerSample = *WaveInfo.pBitsPerSample;
                        uint32 DataSize = WaveInfo.SampleDataSize;
                        const uint8* PCMData = WaveInfo.SampleDataStart;

                        if (BitsPerSample != 16)
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Only 16-bit WAV files are supported. Provided: %d bits"), BitsPerSample);
                            return;
                        }

                        Sound->NumChannels = NumChannels;
                        Sound->SetSampleRate(SampleRate);
                        Sound->Duration = static_cast<float>(DataSize) / (SampleRate * NumChannels * (BitsPerSample / 8));
                        Sound->RawPCMDataSize = DataSize;

                        //  опируем PCM данные
                        Sound->RawPCMData = static_cast<uint8*>(FMemory::Malloc(DataSize));
                        FMemory::Memcpy(Sound->RawPCMData, PCMData, DataSize);

                        Sound->bProcedural = false;
                        Sound->SoundGroup = SOUNDGROUP_Default;
                        Sound->DecompressionType = DTYPE_Setup;

                        SoundData->Init(PathPieces[PathPieces.Num() - 4], Data->Category, Data->SubCategory, Data->id, FileTag, NumChannels, SampleRate, BitsPerSample, DataSize, PCMData);

                        Saver->AddDataToBinArray<USoundData>(SoundData, Value, FullSoundFilePath);

                        SoundsContainer->AddSound(PathPieces[PathPieces.Num() - 4], Data->Category, Data->SubCategory, Data->id, FileTag, Sound);
                    }
                }

                TArray<FString> SavPathPieces;
                FirstLacalFilePath.ParseIntoArray(SavPathPieces, TEXT("/"));

                FString ModuleSavPath = FPaths::ProjectDir();

                for (int i = 0; i < SavPathPieces.Num() - 3; i++) {
                    ModuleSavPath += SavPathPieces[i];
                    ModuleSavPath += "/";
                }

                ModuleSavPath += "sav/Sounds.sav";

                Saver->SaveBinArray(ModuleSavPath);
            }
        }

        AsyncTask(ENamedThreads::GameThread, [this]() {
            ChangeTextOfTheDownloadDetails.Broadcast(FString("Data loading complite"), FColor::Green);
            LoadingComplet.Broadcast();
            });
        });
}

void USoundPreparer::PrepareAllExpeditionInteractionObjectsSounds(
    UPARAM(ref)UExpeditionInteractionObjectContainer* ObjectContainer, UPARAM(ref)USoundContainer* SoundsContainer)
{
    PrepareAllSounds<UExpeditionInteractionObjectContainer, UExpeditionInteractionObjectData>(ObjectContainer, SoundsContainer);
}