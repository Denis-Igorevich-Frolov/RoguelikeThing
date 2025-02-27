// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Containers/ExpeditionInteractionObjectContainer.h"
#include "SpritesPreparer.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(SpritesPreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfThePreparSpritesDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPreparingSpritesCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API USpritesPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void PrepareAllExpeditionInteractionObjectsSprites(
		UPARAM(ref)UExpeditionInteractionObjectContainer* ExpeditionInteractionObjectContainer, TArray<FString> ModsDirWithInteractionObjects);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfThePreparSpritesDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FPreparingSpritesCompleteDelegate LoadingComplet;
};
