// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundContainer.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpecificObjectSounds
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, USoundWave*> SoundTagsArray;
};

USTRUCT(BlueprintType)
struct FSoundSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FSpecificObjectSounds> SpecificObjectSoundsArray;
};

//Структура категории объектов взаимодействия
USTRUCT(BlueprintType)
struct FSoundCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FSoundSubCategory> SoundSubCategorysArray;
};

USTRUCT(BlueprintType)
struct FSoundModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FSoundCategory> SoundCategorysArray;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API USoundContainer : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<FString, FSoundModule> SoundsArray;

public:
	UFUNCTION(BlueprintCallable)
	void AddSound(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString SoundTag, USoundWave* Sound);

	UFUNCTION(BlueprintCallable)
	USoundWave* FindSound(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString SoundTag);
};
