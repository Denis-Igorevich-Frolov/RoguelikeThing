// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractList.generated.h"

/************************************************************************************************************
 * Данный класс является базовым для всех прочих списков контента xml файлов, представленных по умолчанию.
 ************************************************************************************************************/

UCLASS(Abstract)
class ROGUELIKETHING_API UAbstractList : public UObject
{
	GENERATED_BODY()

public:
    //Получение текста xml файла по его имени
    UFUNCTION()
    virtual FString GetXmlText(FString FileName) PURE_VIRTUAL(UAbstractList::GetXmlList, return FString{};);

    //Получение списка имён xml файлов всех объектов в модуле
    UFUNCTION()
    virtual TArray<FString> GetModuleFilesArray() PURE_VIRTUAL(UAbstractList::GetXmlList, return TArray<FString>{};);
};
