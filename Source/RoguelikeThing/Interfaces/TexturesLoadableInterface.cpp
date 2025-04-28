// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Interfaces/TexturesLoadableInterface.h"

DEFINE_LOG_CATEGORY(TexturesLoadable_Interface);

bool ITexturesLoadableInterface::LoadTextures(FXmlNode* ResourcesNode, UAbstractData* Data, FString XMLFilePath, bool IsModDir, FString ModuleName, FString TextureModulesRoot)
{
    FXmlNode* TexturesNode = ResourcesNode->FindChildNode("Textures");
    if (!TexturesNode) {
        UE_LOG(TexturesLoadable_Interface, Error, TEXT("!!! An error occurred in the TexturesLoadableInterface class in the LoadTextures function - Failed to extract Textures node from %s node from file %s"), *ResourcesNode->GetTag(), *XMLFilePath);
        return false;
    }

    //Получение списка путей до текстур, которые связанны с данным объектом
    TArray<FXmlNode*> Textures = TexturesNode->GetChildrenNodes();
    for (FXmlNode* Texture : Textures) {
        if (!Texture) {
            UE_LOG(TexturesLoadable_Interface, Error, TEXT("!!! An error occurred in the TexturesLoadableInterface class in the LoadTextures function - Texture node from %s node from file %s is not valid"), *ResourcesNode->GetTag(), *XMLFilePath);
            return false;
        }

        if (IsModDir) {
            TArray<FString> PathPeces;
            XMLFilePath.ParseIntoArray(PathPeces, TEXT("/"));

            Data->TexturePaths.Add(Texture->GetTag(),
                *FString("Mods/" + PathPeces[PathPeces.Num() - 5] + "/" + TextureModulesRoot  + "/" + ModuleName + "/textures/" +
                    Data->id + "/" + Texture->GetContent()));

            UE_LOG(TexturesLoadable_Interface, Error, TEXT("%s"), *FString("Mods/" + PathPeces[PathPeces.Num() - 5] + "/" + TextureModulesRoot + "/" + ModuleName + "/textures/" +
                Data->id + "/" + Texture->GetContent()));
        }
        else {
            Data->TexturePaths.Add(Texture->GetTag(),
                *FString("Data/" + TextureModulesRoot + "/" + ModuleName + "/textures/" + Data->id + "/" + Texture->GetContent()));

            UE_LOG(TexturesLoadable_Interface, Error, TEXT("%s"), *FString("Data/" + TextureModulesRoot + "/" + ModuleName + "/textures/" + Data->id + "/" + Texture->GetContent()));
        }
    }

    return true;
}
