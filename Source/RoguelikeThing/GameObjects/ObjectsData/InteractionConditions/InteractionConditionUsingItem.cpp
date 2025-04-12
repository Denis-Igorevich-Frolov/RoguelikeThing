// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/InteractionConditionUsingItem.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <Kismet/GameplayStatics.h>

void UInteractionConditionUsingItem::Init(FString itemID, int quantity, FString lackOfQuantityText)
{
    ItemID = itemID;
    Quantity = quantity;
    LackOfQuantityText = lackOfQuantityText;
}

CheckConditionResult UInteractionConditionUsingItem::CheckCondition()
{
    UPROPERTY()
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    if (MyGameInstance) {
        if (MyGameInstance->CurrentCallingItemID == ItemID) {
            if (MyGameInstance->CurrentCallingItemQuantity >= Quantity) {
                MyGameInstance->CurrentCallingItemQuantity = MyGameInstance->CurrentCallingItemQuantity - Quantity;
                return CheckConditionResult::CONFIRMED;
            }
            else {
                return CheckConditionResult::MISSING_ITEMS;
            }
        }
    }

    return CheckConditionResult::FAILED;
}
