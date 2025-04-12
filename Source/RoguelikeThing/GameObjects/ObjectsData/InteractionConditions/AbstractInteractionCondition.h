// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractInteractionCondition.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ConditionType : uint8 {
	REGULAR UMETA(DisplayName = "Regular"),
	USE_ITEM UMETA(DisplayName = "Use item")
};

UENUM(BlueprintType)
enum class CheckConditionResult : uint8 {
	CONFIRMED UMETA(DisplayName = "Confirmed"),
	FAILED UMETA(DisplayName = "Failed"),
	MISSING_ITEMS UMETA(DisplayName = "Missing items")
};


UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractInteractionCondition : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	ConditionType ConditionType;

	UFUNCTION(BlueprintCallable)
	virtual CheckConditionResult CheckCondition();
};
