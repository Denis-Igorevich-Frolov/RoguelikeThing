// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

/**
 * 
 */
class ExpeditionInteractionObjectsList
{
public:
    TMap<FString, FString>ExpeditionInteractionObjectsXMLs{
        {
            "WoodChest.xml",

            FString(UTF8_TO_TCHAR(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
				"\n"
				"<root>\n"
				"	<id>WoodChest</id>\n"
				"	<Name>Деревянный сундук</Name>\n"
				"	<InteractionText>Крепкий и тяжёлый деревянный сундук. На его лицевой стороне зияет замочная скважина.</InteractionText>\n"
				"	<TermsOfInteractions>\n"
				"		<Regular>\n"
				"			<EventsText>Сундук заперт, так просто его не открыть.</EventsText>\n"
				"			<Events></Events>\n"
				"		</Regular>\n"
				"	</TermsOfInteractions>\n"
				"</root>\n"
            ))
        }
    };
};
