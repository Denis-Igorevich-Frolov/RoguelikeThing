// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Lists/InventoryItemsList.h"

//Получение текста xml файла по его имени
FString UInventoryItemsList::GetXmlText(FString FileName)
{
    TArray<FString> Keys;
    InventoryItemsXMLs.GenerateKeyArray(Keys);

    if (Keys.Find(FileName) == INDEX_NONE) {
        return "";
    }

    return *InventoryItemsXMLs.Find(FileName);
}

//Получение списка имён xml файлов всех объектов в модуле
TArray<FString> UInventoryItemsList::GetModuleFilesArray()
{
    TArray<FString> Keys;
    InventoryItemsXMLs.GenerateKeyArray(Keys);

    return Keys;
}
