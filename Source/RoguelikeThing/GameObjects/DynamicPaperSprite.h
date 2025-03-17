// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperSprite.h"
#include "DynamicPaperSprite.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (DisplayThumbnail = "true"))
class ROGUELIKETHING_API UDynamicPaperSprite : public UPaperSprite
{
	GENERATED_BODY()

public:
	UDynamicPaperSprite(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	void SetTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot);
	
	UFUNCTION(BlueprintCallable)
	void SetDefaultMaterial(UMaterialInterface* Material);
};
