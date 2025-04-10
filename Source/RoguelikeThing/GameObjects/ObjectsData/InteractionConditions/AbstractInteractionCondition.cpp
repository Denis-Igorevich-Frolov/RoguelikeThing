// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/AbstractInteractionCondition.h"

UAbstractInteractionCondition::~UAbstractInteractionCondition()
{
    UE_LOG(LogTemp, Log, TEXT("*** ya sdoh"));
}

bool UAbstractInteractionCondition::CheckCondition()
{
    return true;
}
