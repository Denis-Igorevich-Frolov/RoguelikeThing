// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <XmlNode.h>
#include <RoguelikeThing/GameObjects/ObjectsData/AbstractData.h>
#include "SoundsLoadableInterface.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(SoundsLoadable_Interface, Log, All);

UINTERFACE(MinimalAPI)
class USoundsLoadableInterface : public UInterface
{
	GENERATED_BODY()
};

class ROGUELIKETHING_API ISoundsLoadableInterface
{
	GENERATED_BODY()

public:
	bool LoadSounds(FXmlNode* ResourcesNode, UAbstractData* Data, FString XMLFilePath, bool IsModDir, FString ModuleName, FString SoundModulesRoot);
};
