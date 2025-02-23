// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"

FString UExpeditionInteractionObjectsList::GetXmlText(FString FileName)
{
    TArray<FString> Keys;
    ExpeditionInteractionObjectsXMLs.GenerateKeyArray(Keys);

    if (Keys.Find(FileName) == INDEX_NONE) {
        return "";
    }
    
    return *ExpeditionInteractionObjectsXMLs.Find(FileName);
}

TArray<FString> UExpeditionInteractionObjectsList::GetModuleFilesArray()
{
    TArray<FString> Keys;
    ExpeditionInteractionObjectsXMLs.GenerateKeyArray(Keys);

    return Keys;
}