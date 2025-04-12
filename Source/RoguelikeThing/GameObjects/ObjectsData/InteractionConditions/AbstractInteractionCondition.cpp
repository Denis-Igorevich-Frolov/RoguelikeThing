// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/AbstractInteractionCondition.h"

CheckConditionResult UAbstractInteractionCondition::CheckCondition()
{
    return CheckConditionResult::CONFIRMED;
}
