// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ExpeditionInteractionObjectData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FInteractionEvent
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FInteractionCondition
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, SaveGame)
	FString EventsText;

	UPROPERTY(VisibleAnywhere, SaveGame)
	TArray<FInteractionEvent> InteractionEvents;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, SaveGame)
	FString id;

	UPROPERTY(VisibleAnywhere, SaveGame)
	FString Name;

	UPROPERTY(VisibleAnywhere, SaveGame)
	FString InteractionText;

	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FInteractionCondition> TermsOfInteractions;
};
