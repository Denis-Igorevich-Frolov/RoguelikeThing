// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractData.generated.h"

/*********************************************************************************************************************************
 * ������ ����� �������� ������� ������� �������� ���� ������� � ������� ������ �������� ������� ������� ���������� ����������
 *********************************************************************************************************************************/

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString id;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Category;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString SubCategory;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	FString Name;
};
