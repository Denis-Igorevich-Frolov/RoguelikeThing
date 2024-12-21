// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h>
#include "ExpeditionInteractionObjectSaver.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UExpeditionInteractionObjectSaver : public USaveGame
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	UExpeditionInteractionObjectData* ExpeditionInteractionObjectData;

	//Бинарный массив для сохранения и загрузки переменной TerrainOfTilesContainer
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> BinExpeditionInteractionObjectData;

public:
	//Функция, сериализующая переданную переменную ExpeditionInteractionObjectData
	void SaveExpeditionInteractionObjectData(UExpeditionInteractionObjectData* ExpeditionInteractionObjectData);
	//Функция, десериализующая данные из BinExpeditionInteractionObjectData
	UExpeditionInteractionObjectData* LoadExpeditionInteractionObjectData();
};
