// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ExpeditionInteractionObject.h"
#include <XmlParser/Public/XmlFile.h>

// Sets default values
AExpeditionInteractionObject::AExpeditionInteractionObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExpeditionInteractionObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExpeditionInteractionObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//void AExpeditionInteractionObject::XMLTest()
//{
//    FXmlFile* XmlFile = new FXmlFile("E:/Unreal projects/RoguelikeThing/Data/Expedition interaction objects/WoodChest.xml");
//    FXmlNode* RootNode = XmlFile->GetRootNode();
//    FString AssetContent = RootNode->FindChildNode("to")->GetContent();
//
//    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, AssetContent);
//}

