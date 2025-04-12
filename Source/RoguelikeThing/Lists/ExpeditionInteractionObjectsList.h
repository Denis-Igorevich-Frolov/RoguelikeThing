// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/Lists/AbstractList.h"
#include "ExpeditionInteractionObjectsList.generated.h"

/***************************************************************************************************************
 * Данный класс является списком контента xml файлов всех объектов взаимодействия экспедиции, представленных
 * по умолчанию. При повреждении или утрате исходных файлов, они будут восстановлены по этому списку.
 ***************************************************************************************************************/

UCLASS()
class ROGUELIKETHING_API UExpeditionInteractionObjectsList : public UAbstractList
{
	GENERATED_BODY()

private:
	//Коллекция всех xml файлов объектов по умолчанию. Ключом является имя файла, а значением - его контент
    TMap<FString, FString>ExpeditionInteractionObjectsXMLs{
        {
            "WoodChest.xml",

            FString(UTF8_TO_TCHAR(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				"\n"
				"<root>\n"
				"	<id>WoodChest</id>\n"
				"	<Category>Container</Category>\n"
				"	<SubCategory>Common</SubCategory>\n"
				"	<Name>Деревянный сундук</Name>\n"
				"	<InteractionText>Крепкий и тяжёлый деревянный сундук. На его лицевой стороне зияет замочная скважина.</InteractionText>\n"
				"	<TermsOfInteractions type=\"array\">\n"
				"		<KeyInteraction>\n"
				"			<Conditions type=\"array\">\n"
				"				<Condition>Use item[id:Key, quantity:1, lack of quantity text:Ещё бы немного ключей...]</Condition>\n"
				"			</Conditions>\n"
				"			<EventsText>Ключ с лёгкостью отпирает замок.</EventsText>\n"
				"			<Events type=\"array\"></Events>\n"
				"		</KeyInteraction>\n"
				"\n"
				"		<Regular>\n"
				"			<Conditions type=\"array\">\n"
				"				<Condition>Regular</Condition>\n"
				"			</Conditions>\n"
				"			<EventsText>Сундук заперт, так просто его не открыть.</EventsText>\n"
				"			<Events type=\"array\"></Events>\n"
				"		</Regular>\n"
				"	</TermsOfInteractions>\n"
				"	<Resources type=\"array\">\n"
				"		<Textures type=\"array\">\n"
				"			<Unused>CloseWoodChest.png</Unused>\n"
				"			<Used>OpenWoodChest.png</Used>\n"
				"		</Textures>\n"
				"	</Resources>\n"
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
