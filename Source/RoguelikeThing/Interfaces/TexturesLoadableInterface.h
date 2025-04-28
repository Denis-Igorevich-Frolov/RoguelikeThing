// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include <XmlNode.h>
#include <RoguelikeThing/GameObjects/ObjectsData/AbstractData.h>
#include "TexturesLoadableInterface.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(TexturesLoadable_Interface, Log, All);

UINTERFACE(MinimalAPI)
class UTexturesLoadableInterface : public UInterface
{
	GENERATED_BODY()
};

class ROGUELIKETHING_API ITexturesLoadableInterface
{

	GENERATED_BODY()

protected:
	bool LoadTextures(FXmlNode* ResourcesNode, UAbstractData* Data, FString XMLFilePath, bool IsModDir, FString ModuleName, FString TextureModulesRoot);
};
