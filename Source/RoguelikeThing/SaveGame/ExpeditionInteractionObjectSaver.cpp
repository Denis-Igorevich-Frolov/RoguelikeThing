// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/ExpeditionInteractionObjectSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

void UExpeditionInteractionObjectSaver::SaveExpeditionInteractionObjectData(UExpeditionInteractionObjectData* expeditionInteractionObjectData)
{
    ExpeditionInteractionObjectData = expeditionInteractionObjectData;

    FMemoryWriter InteractionObjectWriter = FMemoryWriter(BinExpeditionInteractionObjectData);
    FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectWriter, false);
    InteractionObjectAr.ArIsSaveGame = true;
    ExpeditionInteractionObjectData->Serialize(InteractionObjectAr);
}

UExpeditionInteractionObjectData* UExpeditionInteractionObjectSaver::LoadExpeditionInteractionObjectData()
{
    FMemoryReader InteractionObjectReader = FMemoryReader(BinExpeditionInteractionObjectData);
    FObjectAndNameAsStringProxyArchive InteractionObjectAr(InteractionObjectReader, false);
    InteractionObjectAr.ArIsSaveGame = true;

    ExpeditionInteractionObjectData = NewObject<UExpeditionInteractionObjectData>();
    ExpeditionInteractionObjectData->Serialize(InteractionObjectAr);

    return ExpeditionInteractionObjectData;
}
