// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/ExpeditionInteractionObjectData.h"

void FInteractionCondition::PrepareConditions()
{
    for (FString Condition : ConditionsText) {
        if (Condition == "Regular") {
            UPROPERTY()
            UAbstractInteractionCondition* ObjectData = nullptr;
            AsyncTask(ENamedThreads::GameThread, [&ObjectData, this]() {
                ObjectData = NewObject<UAbstractInteractionCondition>();
                ObjectData->AddToRoot();
                });

            while (!ObjectData) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            Conditions.Add(ObjectData);
        }
    }
}
