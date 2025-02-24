// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"

//Получение текста xml файла по его имени
FString UExpeditionInteractionObjectsList::GetXmlText(FString FileName)
{
    TArray<FString> Keys;
    ExpeditionInteractionObjectsXMLs.GenerateKeyArray(Keys);

    if (Keys.Find(FileName) == INDEX_NONE) {
        return "";
    }
    
    return *ExpeditionInteractionObjectsXMLs.Find(FileName);
}

//Получение списка имён xml файлов всех объектов в модуле
TArray<FString> UExpeditionInteractionObjectsList::GetModuleFilesArray()
{
    TArray<FString> Keys;
    ExpeditionInteractionObjectsXMLs.GenerateKeyArray(Keys);

    return Keys;
}