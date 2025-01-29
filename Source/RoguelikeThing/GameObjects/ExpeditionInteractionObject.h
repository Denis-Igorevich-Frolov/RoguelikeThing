// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h"
#include "ExpeditionInteractionObject.generated.h"

/**************************************************************************************************
 * Данный класс является классом объекта взаимодействия, находящимся на карте во время экспедиции
 **************************************************************************************************/

UCLASS()
class ROGUELIKETHING_API AExpeditionInteractionObject : public AActor
{
	GENERATED_BODY()

private:
	UExpeditionInteractionObjectData* ExpeditionInteractionObjectData;

public:

	UFUNCTION(BlueprintCallable)
	void SetExpeditionInteractionObjectData(UExpeditionInteractionObjectData* expeditionInteractionObjectData);
	UFUNCTION(BlueprintCallable)
	const UExpeditionInteractionObjectData* GetExpeditionInteractionObjectData();
};
