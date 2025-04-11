// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/InteractionConditionUsingItem.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <Kismet/GameplayStatics.h>

void UInteractionConditionUsingItem::Init(FString itemID, int quantity)
{
    ItemID = itemID;
    Quantity = quantity;
}

bool UInteractionConditionUsingItem::CheckCondition()
{
    UPROPERTY()
    UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

    if (MyGameInstance) {
        return (MyGameInstance->CurrentCallingItemID == ItemID && MyGameInstance->CurrentCallingItemQuantity >= Quantity);
    }
    else {
        return false;
    }
}
