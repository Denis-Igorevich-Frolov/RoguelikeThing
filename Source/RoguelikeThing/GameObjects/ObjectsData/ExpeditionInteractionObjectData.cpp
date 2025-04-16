// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/ExpeditionInteractionObjectData.h"
#include "InteractionConditions/InteractionConditionUsingItem.h"
#include "InteractionEvents/AddItemEvent.h"

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

            if (ConditionPieces.Num() == 2 && ConditionPieces[0] == "Use item") {
                ConditionPieces[1] = ConditionPieces[1].LeftChop(1);

                TArray<FString> ConditionParameters;
                ConditionPieces[1].ParseIntoArray(ConditionParameters, TEXT(","));

                FString ItemModule = "";
                FString ItemCategory = "";
                FString ItemSubCategory = "";
                FString ItemID = "";
                int Quantity = 0;
                FString LackOfQuantityText = "";
                for (FString ConditionParameter : ConditionParameters) {
                    TArray<FString> ConditionParametersPieces;
                    ConditionParameter.ParseIntoArray(ConditionParametersPieces, TEXT(":"));

                    if (ConditionParametersPieces.Num() == 2) {
                        ConditionParametersPieces[0].RemoveSpacesInline();
                        ConditionParametersPieces[0] = ConditionParametersPieces[0].ToLower();

                        if (ConditionParametersPieces[0] == "module") {
                            ItemModule = ConditionParametersPieces[1];
                        }
                        else if (ConditionParametersPieces[0] == "category") {
                            ItemCategory = ConditionParametersPieces[1];
                        }
                        else if (ConditionParametersPieces[0] == "subcategory") {
                            ItemSubCategory = ConditionParametersPieces[1];
                        }
                        else if (ConditionParametersPieces[0] == "id") {
                            ItemID = ConditionParametersPieces[1];
                        }
                        else if (ConditionParametersPieces[0] == "quantity") {
                            Quantity = FCString::Atoi(*ConditionParametersPieces[1]);
                        }
                        else if (ConditionParametersPieces[0] == "lackofquantitytext") {
                            LackOfQuantityText = ConditionParametersPieces[1];
                        }
                    }
                }

                UPROPERTY()
                UInteractionConditionUsingItem* InteractionCondition = nullptr;
                AsyncTask(ENamedThreads::GameThread, [&InteractionCondition, ItemModule, ItemCategory, ItemSubCategory, ItemID, Quantity, LackOfQuantityText, Parent, this]() {
                    InteractionCondition = NewObject<UInteractionConditionUsingItem>(Parent);
                    InteractionCondition->Init(ItemModule, ItemCategory, ItemSubCategory, ItemID, Quantity, LackOfQuantityText);
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

void FInteractionCondition::PrepareEvents(UObject* Parent)
{
    for (FString Event : InteractionEventsText) {
        TArray<FString> EventPieces;
        Event.ParseIntoArray(EventPieces, TEXT("["));

        if (EventPieces.Num() == 2 && EventPieces[0] == "Add item") {
            EventPieces[1] = EventPieces[1].LeftChop(1);

            TArray<FString> EventParameters;
            EventPieces[1].ParseIntoArray(EventParameters, TEXT(","));

            FString ItemModule = "";
            FString ItemCategory = "";
            FString ItemSubCategory = "";
            FString ItemID = "";
            int Quantity = 0;
            for (FString EventParameter : EventParameters) {
                TArray<FString> EventParametersPieces;
                EventParameter.ParseIntoArray(EventParametersPieces, TEXT(":"));

                if (EventParametersPieces.Num() == 2) {
                    EventParametersPieces[0].RemoveSpacesInline();
                    EventParametersPieces[0] = EventParametersPieces[0].ToLower();

                    if (EventParametersPieces[0] == "module") {
                        ItemModule = EventParametersPieces[1];
                    }
                    else if (EventParametersPieces[0] == "category") {
                        ItemCategory = EventParametersPieces[1];
                    }
                    else if (EventParametersPieces[0] == "subcategory") {
                        ItemSubCategory = EventParametersPieces[1];
                    }
                    else if (EventParametersPieces[0] == "id") {
                        ItemID = EventParametersPieces[1];
                    }
                    else if (EventParametersPieces[0] == "quantity") {
                        Quantity = FCString::Atoi(*EventParametersPieces[1]);
                    }
                }
            }

            UPROPERTY()
            UAddItemEvent* InteractionEvent = nullptr;
            AsyncTask(ENamedThreads::GameThread, [&InteractionEvent, ItemModule, ItemCategory, ItemSubCategory, ItemID, Quantity, Parent, this]() {
                InteractionEvent = NewObject<UAddItemEvent>(Parent);
                InteractionEvent->Init(ItemModule, ItemCategory, ItemSubCategory, ItemID, Quantity);
                });

            while (!InteractionEvent) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            InteractionEvents.Add(InteractionEvent);
        }
    }
}
