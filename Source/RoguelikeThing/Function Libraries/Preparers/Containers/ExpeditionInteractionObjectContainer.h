// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoguelikeThing/GameObjects/ObjectsData/ExpeditionInteractionObjectData.h"
#include "ExpeditionInteractionObjectContainer.generated.h"

/**
 * 
 */

 //��������� ������������ �������� ��������������
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectSubCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, UExpeditionInteractionObjectData*> InteractionObjectsDataArray;
};

//��������� ��������� �������� ��������������
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FExpeditionInteractionObjectSubCategory> InteractionObjectsSubCategorysArray;
};

//��������� ������ �������� ��������������
USTRUCT(BlueprintType)
struct FExpeditionInteractionObjectModule
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FExpeditionInteractionObjectCategory> InteractionObjectsCategorysArray;
};

 //��������� �� ����� ��������� ��������������
UCLASS(BlueprintType)
class UExpeditionInteractionObjectContainer : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<FString, FExpeditionInteractionObjectModule> InteractionObjectsModulesArray;

public:
	UFUNCTION(BlueprintCallable)
	void AddData(FString ModuleName, FString CategoryName, FString SubCategoryName,
		FString ObjectId, UExpeditionInteractionObjectData* ExpeditionInteractionObjectData);
	UFUNCTION(BlueprintCallable)
	const UExpeditionInteractionObjectData* const FindData(FString ModuleName, FString CategoryName, FString SubCategoryName, FString ObjectId);
	UFUNCTION(BlueprintCallable)
	TMap<FString, UExpeditionInteractionObjectData*> GetAllInteractionObjects();
};
