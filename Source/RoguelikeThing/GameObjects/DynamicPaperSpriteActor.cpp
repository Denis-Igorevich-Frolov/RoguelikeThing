// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/DynamicPaperSpriteActor.h"
#include "RoguelikeThing/GameObjects/DynamicPaperSprite.h"
#include <PaperSpriteComponent.h>

void ADynamicPaperSpriteActor::UpdateTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot)
{
	UPaperSpriteComponent* Render = Super::GetRenderComponent();
	Render->SetMobility(EComponentMobility::Stationary);
	UDynamicPaperSprite* Sprite = dynamic_cast<UDynamicPaperSprite*>(Render->GetSprite());
	if (!Sprite)
		Sprite = NewObject<UDynamicPaperSprite>();
	Sprite->SetTexture(Texture, Pivot);
	Render->SetSprite(Sprite);
}