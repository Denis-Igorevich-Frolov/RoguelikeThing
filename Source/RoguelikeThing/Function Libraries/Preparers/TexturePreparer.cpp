// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/TexturePreparer.h"
#include "ImageUtils.h"

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

        for (FString Key : InteractionObjectsKeys){
            UExpeditionInteractionObjectData* Data = *InteractionObjects.Find(Key);

            TMap<FString, FString> TexturePaths = Data->TexturePaths;
            TArray<FString> Keys;
            TexturePaths.GenerateKeyArray(Keys);
            for (FString Keyy : Keys) {
                FString Value = *TexturePaths.Find(Keyy);
                UE_LOG(TexturePreparer, Log, TEXT("%s: %s"), *Keyy, *Value);
                TArray<FString> PathPieces;
                Value.ParseIntoArray(PathPieces, TEXT("/"));
                UE_LOG(TexturePreparer, Log, TEXT("%s"), *PathPieces[PathPieces.Num() - 3]);

                UPROPERTY()
                UTexture2D* texture = FImageUtils::ImportFileAsTexture2D(*FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + Value));
                ExpeditionInteractionObjectTexturesContainer->AddTexture(PathPieces[PathPieces.Num() - 3], Data->Category, Data->SubCategory, Data->id, Keyy, texture);
            }
            AsyncTask(ENamedThreads::GameThread, [this]() {
                //ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + ModuleName + ".sav"), FColor::FromHex("160124"));
                });
        }

        AsyncTask(ENamedThreads::GameThread, [this]() {
            LoadingComplet.Broadcast();
            });
        });
}