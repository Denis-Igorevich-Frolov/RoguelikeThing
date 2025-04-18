// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/DynamicPaperSpriteActor.h"
#include "RoguelikeThing/GameObjects/DynamicPaperSprite.h"
#include <PaperSpriteComponent.h>

void ADynamicPaperSpriteActor::UpdateTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot)
{
	UPaperSpriteComponent* Render = Super::GetRenderComponent();
	Render->SetMobility(EComponentMobility::Stationary);
	UDynamicPaperSprite* Sprite = dynamic_cast<UDynamicPaperSprite*>(Render->GetSprite());

	if (Sprite) {
		Sprite->MarkPendingKill();
	}

    Sprite = NewObject<UDynamicPaperSprite>();

	Sprite->SetTexture(Texture, Pivot);
	Render->SetSprite(Sprite);
}

FVector2D ADynamicPaperSpriteActor::SpritePivotModeSwitch(ESpritePivotMode::Type Pivot)
{
	FVector2D Position;
	switch (Pivot)
	{
	case ESpritePivotMode::Top_Left:
		Position = FVector2D(0, 0);
		break;
	case ESpritePivotMode::Top_Center:
		Position = FVector2D(0.5 , 0);
		break;
	case ESpritePivotMode::Top_Right:
		Position = FVector2D(1, 0);
		break;
	case ESpritePivotMode::Center_Left:
		Position = FVector2D(0, 0.5);
		break;
	case ESpritePivotMode::Center_Center:
		Position = FVector2D(0.5 , 0.5);
		break;
	case ESpritePivotMode::Center_Right:
		Position = FVector2D(1, 0.5);
		break;
	case ESpritePivotMode::Bottom_Left:
		Position = FVector2D(0, 1);
		break;
	case ESpritePivotMode::Bottom_Center:
		Position = FVector2D(0.5, 1);
		break;
	case ESpritePivotMode::Bottom_Right:
		Position = FVector2D(1, 1);
		break;
	case ESpritePivotMode::Custom:
		Position = FVector2D(0.5, 0.5);
		//!!!
		break;
	default:
		Position = FVector2D(0.5, 0.5);
		break;
	}

	return Position;
}
