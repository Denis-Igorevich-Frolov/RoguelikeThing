// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "DynamicPaperSpriteActor.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, ComponentWrapperClass, meta = (ChildCanTick))
class ROGUELIKETHING_API ADynamicPaperSpriteActor : public APaperSpriteActor
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void UpdateTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot = ESpritePivotMode::Center_Center);
};
