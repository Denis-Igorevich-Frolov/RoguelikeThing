// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/DynamicPaperSprite.h"

UDynamicPaperSprite::UDynamicPaperSprite(const FObjectInitializer& ObjectInitializer)
{
	SpriteCollisionDomain = ESpriteCollisionMode::None;
	PixelsPerUnrealUnit = 1.0f;
}

void UDynamicPaperSprite::SetTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot)
{
	if (Texture){
		float Width = Texture->GetSizeX();
		float Height = Texture->GetSizeY();

		BakedSourceTexture = Texture;
		BakedSourceUV.Set(0, 0);
		BakedSourceDimension.Set(Width, Height);

		FVector2D RawPivot;
		switch (Pivot)
		{
		case ESpritePivotMode::Top_Left:
			RawPivot = FVector2D(BakedSourceUV.X, BakedSourceUV.Y);
			break;
		case ESpritePivotMode::Top_Center:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X * 0.5f, BakedSourceUV.Y);
			break;
		case ESpritePivotMode::Top_Right:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X, BakedSourceUV.Y);
			break;
		case ESpritePivotMode::Center_Left:
			RawPivot = FVector2D(BakedSourceUV.X, BakedSourceUV.Y + BakedSourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Center_Center:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X * 0.5f, BakedSourceUV.Y + BakedSourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Center_Right:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X, BakedSourceUV.Y + BakedSourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Bottom_Left:
			RawPivot = FVector2D(BakedSourceUV.X, BakedSourceUV.Y + BakedSourceDimension.Y);
			break;
		case ESpritePivotMode::Bottom_Center:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X * 0.5f, BakedSourceUV.Y + BakedSourceDimension.Y);
			break;
		case ESpritePivotMode::Bottom_Right:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X, BakedSourceUV.Y + BakedSourceDimension.Y);
			break;
		case ESpritePivotMode::Custom:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X * 0.5f, BakedSourceUV.Y + BakedSourceDimension.Y * 0.5f);
			//!!!
			break;
		default:
			RawPivot = FVector2D(BakedSourceUV.X + BakedSourceDimension.X * 0.5f, BakedSourceUV.Y + BakedSourceDimension.Y * 0.5f);
			break;
		}
		
		FVector2D Pivot = RawPivot;
		Pivot.X = FMath::RoundToFloat(Pivot.X);
		Pivot.Y = FMath::RoundToFloat(Pivot.Y);

		BakedRenderData.Empty(6);

		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - 0.0f, 0.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - 0.0f, 1.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - Height, 1.0f, 1.0f));

		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - 0.0f, 0.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - Height, 1.0f, 1.0f));
		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - Height, 0.0f, 1.0f));
	}
}