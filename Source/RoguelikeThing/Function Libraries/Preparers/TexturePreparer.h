// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Containers/ExpeditionInteractionObjectContainer.h"
#include "Containers/TextureContainer.h"
#include "TexturePreparer.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(TexturePreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfThePreparTexturesDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPreparingTexturesCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UTexturePreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void PrepareAllExpeditionInteractionObjectsTextures(UPARAM(ref)UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer,
		UPARAM(ref)UTextureContainer* ExpeditionInteractionObjectTexturesContainer);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfThePreparTexturesDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FPreparingTexturesCompleteDelegate LoadingComplet;
};
