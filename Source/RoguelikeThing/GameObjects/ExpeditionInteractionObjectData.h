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

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FInteractionEvent> InteractionEvents;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString id;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Name;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FInteractionCondition> TermsOfInteractions;
};
