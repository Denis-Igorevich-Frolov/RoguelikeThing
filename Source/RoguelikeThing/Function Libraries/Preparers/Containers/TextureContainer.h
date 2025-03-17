// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TextureContainer.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(TextureContainer, Log, All);

USTRUCT(BlueprintType)
struct FSpecificObjectTextures
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UTexture2D*> TextureTagsArray;
};

USTRUCT(BlueprintType)
struct FTextureSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FSpecificObjectTextures> SpecificObjectTexturesArray;
};

//Структура категории объектов взаимодействия
USTRUCT(BlueprintType)
struct FTextureCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FTextureSubCategory> TextureSubCategorysArray;
};

USTRUCT(BlueprintType)
struct FTextureModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FTextureCategory> TextureCategorysArray;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UTextureContainer : public UObject
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TMap<FString, FTextureModule> TexturesArray;

public:
	UFUNCTION(BlueprintCallable)
	void AddTexture(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString TextureTag, UTexture2D* Texture);

	UFUNCTION(BlueprintCallable)
	UTexture2D* FindTexture(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString TextureTag);
};
