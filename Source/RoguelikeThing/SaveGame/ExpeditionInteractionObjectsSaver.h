// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h>
#include "ExpeditionInteractionObjectsSaver.generated.h"

/**
 * 
 */
USTRUCT()
struct FBinArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TArray<uint8> BinArray;
};

UCLASS()
class ROGUELIKETHING_API UExpeditionInteractionObjectsSaver : public USaveGame
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FBinArrayWrapper> BinExpeditionInteractionObjectsData;

	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FString> XMLFilesHash;

public:

	void AddExpeditionInteractionObjectDataToBinArray(UExpeditionInteractionObjectData* ExpeditionInteractionObjectData, FString FilePath);
	TMap<FString, UExpeditionInteractionObjectData*> GetExpeditionInteractionObjectsData();
	void SaveBinArray(FString FilePath);
	void LoadBinArray(FString FilePath);
	bool CheckHashChange();
	int GetBinArraySize();
	void ClearArray();
};
