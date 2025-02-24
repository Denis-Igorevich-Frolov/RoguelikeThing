// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/GameObjects/AbstractData.h"
#include "ExpeditionInteractionObjectData.generated.h"

/*********************************************************************************************************************************
 * Данный класс является классом полного описания всех свойств и ивентов одного отдельно взятого объекта интеракции экспедиции
 *********************************************************************************************************************************/

//Структура ивента, связанного с объектом.		!!!!!На данный момент находящаяся в состоянии зачаточном и существующая для подготовки методов, сопряжённых с её будущим функционалом
USTRUCT(BlueprintType)
struct FInteractionEvent
{
	GENERATED_BODY()
};

//Структура условия взаимодействия с объектом.  !!!!!На данный момент находящаяся в состоянии зачаточном и существующая для подготовки методов, сопряжённых с её будущим функционалом
USTRUCT(BlueprintType)
struct FInteractionCondition
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TArray<FString> Conditions;

	//Текст, который будет выведен при выполнении условия интеракции
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	//Эвенты, которые будут выведены при выполнении условия интеракции
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FInteractionEvent> InteractionEvents;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectData : public UAbstractData
{
	GENERATED_BODY()
	
public:
	//Текст, который будет выведен при попытке взаимодействия с объектом
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString InteractionText;

	//Все возможные условия взаимодействия с объектом
	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FInteractionCondition> TermsOfInteractions;
};
