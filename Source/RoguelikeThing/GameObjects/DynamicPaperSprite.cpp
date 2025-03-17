// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/DynamicPaperSprite.h"

UDynamicPaperSprite::UDynamicPaperSprite(const FObjectInitializer& ObjectInitializer)
{
	SpriteCollisionDomain = ESpriteCollisionMode::None;
	PixelsPerUnrealUnit = 1.0f;
	RenderGeometry.GeometryType = ESpritePolygonMode::SourceBoundingBox;
}

void UDynamicPaperSprite::SetTexture(UTexture2D* Texture, ESpritePivotMode::Type Pivot)
{
	if (Texture){
		float Width = Texture->GetSizeX();
		float Height = Texture->GetSizeY();

		SourceTexture = Texture;
		SourceUV.Set(0, 0);
		SourceDimension.Set(Width, Height);

		FVector2D RawPivot;
		switch (Pivot)
		{
		case ESpritePivotMode::Top_Left:
			RawPivot = FVector2D(SourceUV.X, SourceUV.Y);
			break;
		case ESpritePivotMode::Top_Center:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X * 0.5f, SourceUV.Y);
			break;
		case ESpritePivotMode::Top_Right:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X, SourceUV.Y);
			break;
		case ESpritePivotMode::Center_Left:
			RawPivot = FVector2D(SourceUV.X, SourceUV.Y + SourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Center_Center:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X * 0.5f, SourceUV.Y + SourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Center_Right:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X, SourceUV.Y + SourceDimension.Y * 0.5f);
			break;
		case ESpritePivotMode::Bottom_Left:
			RawPivot = FVector2D(SourceUV.X, SourceUV.Y + SourceDimension.Y);
			break;
		case ESpritePivotMode::Bottom_Center:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X * 0.5f, SourceUV.Y + SourceDimension.Y);
			break;
		case ESpritePivotMode::Bottom_Right:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X, SourceUV.Y + SourceDimension.Y);
			break;
		case ESpritePivotMode::Custom:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X * 0.5f, SourceUV.Y + SourceDimension.Y * 0.5f);
			//!!!
			break;
		default:
			RawPivot = FVector2D(SourceUV.X + SourceDimension.X * 0.5f, SourceUV.Y + SourceDimension.Y * 0.5f);
			break;
		}
		
		FVector2D Pivot = RawPivot;
		Pivot.X = FMath::RoundToFloat(Pivot.X);
		Pivot.Y = FMath::RoundToFloat(Pivot.Y);

		BakedRenderData.Empty(6);

		// Uses 2 triangles to describe the mesh.
		// Each vertex is in the format: [ImageX relative to Pivot, ImageY relative to Pivot, U, V]
		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - 0.0f, 0.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - 0.0f, 1.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - Height, 1.0f, 1.0f));

		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - 0.0f, 0.0f, 0.0f));
		BakedRenderData.Add(FVector4(Width - Pivot.X, Pivot.Y - Height, 1.0f, 1.0f));
		BakedRenderData.Add(FVector4(0.0f - Pivot.X, Pivot.Y - Height, 0.0f, 1.0f));
	}
}

void UDynamicPaperSprite::SetDefaultMaterial(UMaterialInterface* Material)
{
	DefaultMaterial = Material;
}
