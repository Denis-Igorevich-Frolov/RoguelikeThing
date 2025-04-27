// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Containers/ExpeditionInteractionObjectContainer.h"
#include "Containers/SoundContainer.h"
#include "SoundPreparer.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(SoundPreparer, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FChangeTextOfThePreparSoundsDetailsDelegate, FString, Text, FColor, Color);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPreparingSoundsCompleteDelegate);

UCLASS(BlueprintType)
class ROGUELIKETHING_API USoundPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	template<typename GameObjectContainer, typename GameObjectData>
	void PrepareAllSounds(GameObjectContainer* ObjectContainer, USoundContainer* SoundsContainer);
	
public:
	UFUNCTION(BlueprintCallable)
	void PrepareAllExpeditionInteractionObjectsSounds(UPARAM(ref)UExpeditionInteractionObjectContainer* ObjectContainer, UPARAM(ref)USoundContainer* SoundsContainer);

	UPROPERTY(BlueprintAssignable)
	FChangeTextOfThePreparSoundsDetailsDelegate ChangeTextOfTheDownloadDetails;

	UPROPERTY(BlueprintAssignable)
	FPreparingSoundsCompleteDelegate LoadingComplet;
};
