// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/GameObjects/ObjectsData/AbstractData.h"
#include "InteractionConditions/AbstractInteractionCondition.h"
#include "InteractionEvents/AbstractInteractionEvent.h"
#include "ExpeditionInteractionObjectData.generated.h"

/*********************************************************************************************************************************
 * Данный класс является классом полного описания всех свойств и ивентов одного отдельно взятого объекта интеракции экспедиции
 *********************************************************************************************************************************/

//Структура условия взаимодействия с объектом.
USTRUCT(BlueprintType)
struct FInteractionCondition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TArray<FString> ConditionsText;

	UPROPERTY(BlueprintReadWrite)
	TArray<UAbstractInteractionCondition*> Conditions;

	//Текст, который будет выведен при выполнении условия интеракции
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	//Эвенты, которые будут выведены при выполнении условия интеракции
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TArray<FString> InteractionEventsText;
	
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TArray<UAbstractInteractionEvent*> InteractionEvents;

	void PrepareConditions(UObject* Parent);
	void PrepareEvents(UObject* Parent);
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectData : public UAbstractData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString ModuleLocalPath;

	//Текст, который будет выведен при попытке взаимодействия с объектом
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	//Все возможные условия взаимодействия с объектом
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FInteractionCondition> TermsOfInteractions;
};
