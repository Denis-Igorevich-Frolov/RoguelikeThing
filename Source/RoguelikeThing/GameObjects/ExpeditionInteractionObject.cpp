// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ExpeditionInteractionObject.h"
//#include <XmlParser/Public/XmlFile.h>

//void AExpeditionInteractionObject::XMLTest()
//{
//    FXmlFile* XmlFile = new FXmlFile("E:/Unreal projects/RoguelikeThing/Data/Expedition interaction objects/WoodChest.xml");
//    FXmlNode* RootNode = XmlFile->GetRootNode();
//    FString AssetContent = RootNode->FindChildNode("to")->GetContent();
//
//    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, AssetContent);
//}

void AExpeditionInteractionObject::SetExpeditionInteractionObjectData(UExpeditionInteractionObjectData* expeditionInteractionObjectData)
{
    ExpeditionInteractionObjectData = expeditionInteractionObjectData;
}

const UExpeditionInteractionObjectData* AExpeditionInteractionObject::SetExpeditionInteractionObjectData()
{
    return ExpeditionInteractionObjectData;
}
