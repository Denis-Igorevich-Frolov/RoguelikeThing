// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/InteractionEvents/MakeObjectUsedEvent.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <Kismet/GameplayStatics.h>

void UMakeObjectUsedEvent::DoThing()
{
    UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    GameInstance->MakeObjectUsed();
}
