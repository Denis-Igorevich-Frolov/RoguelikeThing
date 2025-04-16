// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/InteractionConditionUsingItem.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <Kismet/GameplayStatics.h>

void UInteractionConditionUsingItem::Init(FString itemModule, FString itemCategory, FString itemSubCategory, FString itemID, int quantity, FString lackOfQuantityText)
{
    ItemModule = itemModule;
    ItemCategory = itemCategory;
    ItemSubCategory = itemSubCategory;
    ItemID = itemID;
    Quantity = quantity;
    LackOfQuantityText = lackOfQuantityText;
}

CheckConditionResult UInteractionConditionUsingItem::CheckCondition()
{
    UPROPERTY()
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    if (MyGameInstance) {
        FCurrentCallingItemInfo& CurrentCallingItemInfo = MyGameInstance->CurrentCallingItemInfo;
        if (CurrentCallingItemInfo.ItemModule == ItemModule && CurrentCallingItemInfo.ItemCategory == ItemCategory &&
            CurrentCallingItemInfo.ItemSubCategory == ItemSubCategory && CurrentCallingItemInfo.ItemID == ItemID) {
            if (CurrentCallingItemInfo.ItemQuantity >= Quantity) {
                CurrentCallingItemInfo.ItemQuantity = CurrentCallingItemInfo.ItemQuantity - Quantity;
                return CheckConditionResult::CONFIRMED;
            }
            else {
                return CheckConditionResult::MISSING_ITEMS;
            }
        }
    }

    return CheckConditionResult::FAILED;
}
