// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractData.generated.h"

/***************************************************************************************************************************************
 * Данный класс является базовым для всех прочих классов полного описания всех свойств и ивентов одного отдельно взятого объекта игры
 ***************************************************************************************************************************************/

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString id;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Module;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Category;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString SubCategory;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Name;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FString> TexturePaths;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FString> SoundPaths;
};
