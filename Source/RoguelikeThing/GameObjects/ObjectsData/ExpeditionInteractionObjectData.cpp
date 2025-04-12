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

            InteractionCondition->ConditionType = ConditionType::REGULAR;
            Conditions.Add(InteractionCondition);
        }
        else {
            TArray<FString> ConditionPieces;
            Condition.ParseIntoArray(ConditionPieces, TEXT("["));

            if ((ConditionPieces.Num() == 2 || ConditionPieces.Num() == 3) && ConditionPieces[0] == "Use item") {
                ConditionPieces[1] = ConditionPieces[1].LeftChop(1);

                TArray<FString> ConditionParameters;
                ConditionPieces[1].ParseIntoArray(ConditionParameters, TEXT(","));

                FString ItemID = "";
                int Quantity = 0;
                FString LackOfQuantityText = "";
                for (FString ConditionParameter : ConditionParameters) {
                    TArray<FString> ConditionParametersPieces;
                    ConditionParameter.ParseIntoArray(ConditionParametersPieces, TEXT(":"));

                    if (ConditionParametersPieces.Num() == 2) {
                        ConditionParametersPieces[0].RemoveSpacesInline();

                        if (ConditionParametersPieces[0] == "id") {
                            ItemID = ConditionParametersPieces[1];
                        }
                        else if (ConditionParametersPieces[0] == "quantity") {
                            Quantity = FCString::Atoi(*ConditionParametersPieces[1]);
                        }
                        else if (ConditionParametersPieces[0] == "Lackofquantitytext") {
                            LackOfQuantityText = ConditionParametersPieces[1];
                        }
                    }
                }

                UPROPERTY()
                UInteractionConditionUsingItem* InteractionCondition = nullptr;
                AsyncTask(ENamedThreads::GameThread, [&InteractionCondition, ItemID, Quantity, LackOfQuantityText, Parent, this]() {
                    InteractionCondition = NewObject<UInteractionConditionUsingItem>(Parent);
                    InteractionCondition->Init(ItemID, Quantity, LackOfQuantityText);
                    });

                while (!InteractionCondition) {
                    FPlatformProcess::SleepNoStats(0.0f);
                }

                InteractionCondition->ConditionType = ConditionType::USE_ITEM;

                Conditions.Add(InteractionCondition);
            }
        }
    }
}
