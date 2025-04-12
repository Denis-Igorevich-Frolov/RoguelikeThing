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

UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractInteractionCondition : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	ConditionType ConditionType;

	UFUNCTION(BlueprintCallable)
	virtual bool CheckCondition();
};
