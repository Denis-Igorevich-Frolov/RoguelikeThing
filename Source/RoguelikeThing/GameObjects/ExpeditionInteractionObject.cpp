// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ExpeditionInteractionObject.h"

void AExpeditionInteractionObject::SetExpeditionInteractionObjectData(UExpeditionInteractionObjectData* expeditionInteractionObjectData)
{
    ExpeditionInteractionObjectData = expeditionInteractionObjectData;
}

const UExpeditionInteractionObjectData* AExpeditionInteractionObject::GetExpeditionInteractionObjectData()
{
    return ExpeditionInteractionObjectData;
}
