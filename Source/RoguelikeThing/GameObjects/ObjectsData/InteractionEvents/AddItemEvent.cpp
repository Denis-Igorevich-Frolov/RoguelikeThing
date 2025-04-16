// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionEvents/AddItemEvent.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <Kismet/GameplayStatics.h>

void UAddItemEvent::Init(FString itemModule, FString itemCategory, FString itemSubCategory, FString itemID, int quantity)
{
    ItemModule = itemModule;
    ItemCategory = itemCategory;
    ItemSubCategory = itemSubCategory;
    ItemID = itemID;
    Quantity = quantity;
}

void UAddItemEvent::DoThing()
{
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    GameInstance->AddItemToInventory(ItemModule, ItemCategory, ItemSubCategory, ItemID, Quantity);
}
