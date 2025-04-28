// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Interfaces/SoundsLoadableInterface.h"

DEFINE_LOG_CATEGORY(SoundsLoadable_Interface);

bool ISoundsLoadableInterface::LoadSounds(FXmlNode* ResourcesNode, UAbstractData* Data, FString XMLFilePath, bool IsModDir, FString ModuleName, FString SoundModulesRoot)
{
    FXmlNode* SoundsNode = ResourcesNode->FindChildNode("Sounds");
    if (!SoundsNode) {
        UE_LOG(SoundsLoadable_Interface, Error, TEXT("!!! An error occurred in the ISoundsLoadableInterface class in the LoadSounds function - Failed to extract Sounds node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);
        return false;
    }

    TArray<FXmlNode*> Sounds = SoundsNode->GetChildrenNodes();
    for (FXmlNode* Sound : Sounds) {
        if (!Sound) {
            UE_LOG(SoundsLoadable_Interface, Error, TEXT("!!! An error occurred in the ISoundsLoadableInterface class in the LoadSounds function - Sound node from %s node from file %s is not valid"), *ResourcesNode->GetTag(), *XMLFilePath);
            return false;
        }

        if (IsModDir) {
            TArray<FString> PathPeces;
            XMLFilePath.ParseIntoArray(PathPeces, TEXT("/"));

            Data->SoundPaths.Add(Sound->GetTag(),
                *FString("Mods/" + PathPeces[PathPeces.Num() - 5] + "/" + SoundModulesRoot + "/" + ModuleName + "/sounds/" + Data->id + "/" + Sound->GetContent()));

            UE_LOG(SoundsLoadable_Interface, Error, TEXT("%s"), *FString("Mods/" + PathPeces[PathPeces.Num() - 5] + "/" + SoundModulesRoot + "/" + ModuleName + "/sounds/" +
                Data->id + "/" + Sound->GetContent()));
        }
        else {
            Data->SoundPaths.Add(Sound->GetTag(),
                *FString("Data/" + SoundModulesRoot + "/" + ModuleName + "/sounds/" + Data->id + "/" + Sound->GetContent()));

            UE_LOG(SoundsLoadable_Interface, Error, TEXT("%s"), *FString("Data/" + SoundModulesRoot + "/" + ModuleName + "/sounds/" + Data->id + "/" + Sound->GetContent()));
        }
    }

    return true;
}
