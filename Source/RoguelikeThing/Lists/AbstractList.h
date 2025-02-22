// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractList.generated.h"

/***************************************************************************************************************
 * Данный класс является списком контента xml файлов всех итемов инвентаря, представленных по умолчанию.
 * При повреждении или утрате исходных файлов, они будут восстановлены по этому списку.
 ***************************************************************************************************************/

UCLASS(Abstract)
class ROGUELIKETHING_API UAbstractList : public UObject
{
	GENERATED_BODY()

public:
    UFUNCTION()
    virtual const TMap<FString, FString> GetXmlList() PURE_VIRTUAL(UAbstractList::GetXmlList, return TMap<FString, FString>{};);
};
