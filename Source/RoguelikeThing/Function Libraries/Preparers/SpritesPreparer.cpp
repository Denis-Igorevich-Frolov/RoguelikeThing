// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/SpritesPreparer.h"

DEFINE_LOG_CATEGORY(SpritesPreparer);

void USpritesPreparer::PrepareAllExpeditionInteractionObjectsSprites(
    UPARAM(ref)UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, TArray<FString> ModsDirWithInteractionObjects)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ExpeditionInteractionObjectContainer, ModsDirWithInteractionObjects, this]() {
        if (!ExpeditionInteractionObjectContainer) {
            UE_LOG(SpritesPreparer, Log, TEXT("!!!!!!!"));
            return;
        }

        TMap<FString, UExpeditionInteractionObjectData*> InteractionObjects = ExpeditionInteractionObjectContainer->GetAllInteractionObjects();
        TArray<FString> InteractionObjectsKeys;
        InteractionObjects.GenerateKeyArray(InteractionObjectsKeys);

        for (FString Key : InteractionObjectsKeys){
            UExpeditionInteractionObjectData* Data = *InteractionObjects.Find(Key);

            AsyncTask(ENamedThreads::GameThread, [this]() {
                //ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + ModuleName + ".sav"), FColor::FromHex("160124"));
                });
        }

        AsyncTask(ENamedThreads::GameThread, [this]() {
            LoadingComplet.Broadcast();
            });
        });
}
