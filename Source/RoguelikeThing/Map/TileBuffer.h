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
	UPROPERTY()
	TArray<UUserWidget*> TileBuf;
	int MaxSize = 100;

	int MapTileLength = -1;
	UPROPERTY()
	UUniformGridPanel* TilesGridPanel;
	UPROPERTY()
	UMapEditor* MapEditor;
	UPROPERTY()
	UClass* MapTileClass;
	UPROPERTY()
	UClass* CellClass;

	bool IsInit = false;

public:
	UFUNCTION(BlueprintCallable)
	void Init(int mapTileLength, UUniformGridPanel* refTilesGridPanel, UClass* refMapTileClass, UClass* refCellClass, UMapEditor* refMapEditor);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetTile();

	UFUNCTION(BlueprintCallable)
	void AddTile(UUserWidget* Tile);

	UFUNCTION(BlueprintCallable)
	bool ScoreToMaximum();

	UFUNCTION(BlueprintCallable)
	void Clear();

	UFUNCTION(BlueprintCallable)
	int BufSize();
	
	UFUNCTION(BlueprintCallable)
	int GetMaxSize();
};
