// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/ExpeditionInteractionObjectData.h"
#include "InteractionConditions/InteractionConditionUsingItem.h"

void FInteractionCondition::PrepareConditions(UObject* Parent)
{
    for (FString Condition : ConditionsText) {
        if (Condition == "Regular") {
            UPROPERTY()
            UAbstractInteractionCondition* InteractionCondition = nullptr;
            AsyncTask(ENamedThreads::GameThread, [&InteractionCondition, Parent, this]() {
                InteractionCondition = NewObject<UAbstractInteractionCondition>(Parent);
                });

            while (!InteractionCondition) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            Conditions.Add(InteractionCondition);
        }
        else {
            TArray<FString> ConditionPieces;
            Condition.ParseIntoArray(ConditionPieces, TEXT("["));

            if (ConditionPieces.Num() == 2 && ConditionPieces[0] == "Use item") {
                ConditionPieces.Last() = ConditionPieces.Last().LeftChop(1);
                ConditionPieces.Last().RemoveSpacesInline();

                TArray<FString> ConditionParameters;
                ConditionPieces.Last().ParseIntoArray(ConditionParameters, TEXT(","));

                FString ItemID = "";
                int Quantity = 0;
                for (FString ConditionParameter : ConditionParameters) {
                    TArray<FString> ConditionParametersPieces;
                    ConditionParameter.ParseIntoArray(ConditionParametersPieces, TEXT(":"));

                    if (ConditionParametersPieces.Num() == 2) {
                        if (ConditionParametersPieces[0] == "id") {
                            ItemID = ConditionParametersPieces.Last();
                        }
                        else if (ConditionParametersPieces[0] == "quantity") {
                            Quantity = FCString::Atoi(*ConditionParametersPieces.Last());
                        }
                    }
                }

                UPROPERTY()
                UInteractionConditionUsingItem* InteractionCondition = nullptr;
                AsyncTask(ENamedThreads::GameThread, [&InteractionCondition, ItemID, Quantity, Parent, this]() {
                    InteractionCondition = NewObject<UInteractionConditionUsingItem>(Parent);
                    InteractionCondition->Init(ItemID, Quantity);
                    });

                while (!InteractionCondition) {
                    FPlatformProcess::SleepNoStats(0.0f);
                }

                Conditions.Add(InteractionCondition);
            }
        }
    }
}
