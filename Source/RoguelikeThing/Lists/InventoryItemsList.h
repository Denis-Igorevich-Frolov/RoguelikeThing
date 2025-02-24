// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Lists/AbstractList.h"
#include "InventoryItemsList.generated.h"

/***************************************************************************************************************
 * Данный класс является списком контента xml файлов всех итемов инвентаря, представленных по умолчанию.
 * При повреждении или утрате исходных файлов, они будут восстановлены по этому списку.
 ***************************************************************************************************************/

UCLASS()
class ROGUELIKETHING_API UInventoryItemsList : public UAbstractList
{
	GENERATED_BODY()

private:
	//Коллекция всех xml файлов объектов по умолчанию. Ключом является имя файла, а значением - его контент
    TMap<FString, FString>InventoryItemsXMLs{
        {
            "Key.xml",

            FString(UTF8_TO_TCHAR(
                "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n"
				"\n"
				"<root>\n"
				"	<id>Key</id>\n"
				"	<Category>Unlocking containers</Category>\n"
				"	<SubCategory>Common</SubCategory>\n"
				"	<Name>Ключ</Name>\n"
				"	<MaximumAmountInStack>10</MaximumAmountInStack>\n"
				"	<AllowedForUseOnExpeditionInteractionObjects>true</AllowedForUseOnExpeditionInteractionObjects>\n"
				"	<UseEvents></UseEvents>\n"
				"</root>\n"
            ))
        }
    };

public:
	//Получение текста xml файла по его имени
	FString GetXmlText(FString FileName) override;
	//Получение списка имён xml файлов всех объектов в модуле
	TArray<FString> GetModuleFilesArray() override;
};
