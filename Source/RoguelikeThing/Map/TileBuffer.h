// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "TileBuffer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TileBuffer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UTileBuffer : public UObject
{
	GENERATED_BODY()
	
private:
	TArray<UUserWidget*> TileBuf;
	int MaxSize = 30;

	int MapTileLength = -1;
	UUniformGridPanel* TilesGridPanel;
	UMapEditor* MapEditor;
	UClass* MapTileClass;
	UClass* CellClass;

	bool IsInit = false;

public:
	UFUNCTION(BlueprintCallable)
	void Init(int mapTileLength, UUniformGridPanel* tilesGridPanel, UClass* mapTileClass, UClass* cellClass, UMapEditor* mapEditor);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetTile();

	UFUNCTION(BlueprintCallable)
	bool ScoreToMaximum();

	UFUNCTION(BlueprintCallable)
	void Clear();
};
