// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Lists/InventoryItemsList.h"

FString UInventoryItemsList::GetXmlText(FString FileName)
{
    TArray<FString> Keys;
    InventoryItemsXMLs.GenerateKeyArray(Keys);

    if (Keys.Find(FileName) == INDEX_NONE) {
        return "";
    }

    return *InventoryItemsXMLs.Find(FileName);
}

TArray<FString> UInventoryItemsList::GetModuleFilesArray()
{
    TArray<FString> Keys;
    InventoryItemsXMLs.GenerateKeyArray(Keys);

    return Keys;
}
