// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "TileBuffer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TileBuffer, Log, All);

UCLASS(BlueprintType)
class ROGUELIKETHING_API UTileBuffer : public UObject
{
	GENERATED_BODY()
	
private:
	//Непосредственно сам буфер
	UPROPERTY()
	TArray<UAbstractTile*> TileBuf;
	//Максимальный размер буфера, больше которого новые тайлы добавляться не будут
	int MaxSize = 100;

	int MapTileLength = -1;
	//Таблица, к которой будут пренадлежать тайлы, созданные в буфере
	UPROPERTY()
	UUniformGridPanel* TilesGridPanel;
	//Указатель на редактор карт, который будет передан в тайлы, создаваемые буфером
	UPROPERTY()
	UMapEditor* MapEditor;
	//Класс, экземплярами которого будут создаваемые в буфере тайлы
	UPROPERTY()
	UClass* MapTileClass;
	//Класс, экземплярами которого будут заполнены тайлы создаваемые в буфере
	UPROPERTY()
	UClass* CellClass;

	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	bool IsInit = false;

public:
	UTileBuffer();

	UFUNCTION(BlueprintCallable)
	void Init(int mapTileLength, UUniformGridPanel* refTilesGridPanel, UClass* refMapTileClass, UClass* refCellClass, UMapEditor* refMapEditor);

	//Получение тайла из буфера, при этом в буфере переданный тайл удаляется
	UFUNCTION(BlueprintCallable)
	UAbstractTile* GetTile();

	//Добавляет переданный тайл в буфер
	UFUNCTION(BlueprintCallable)
	void AddTile(UAbstractTile* Tile);

	//Заполняет буфер новыми тайлами до предела
	UFUNCTION(BlueprintCallable)
	bool FillToMaximum();

	//Полностью очищает буфер
	UFUNCTION(BlueprintCallable)
	void Clear();

	//Функция, возвращающая текущее количество элементов в буфере
	UFUNCTION(BlueprintCallable)
	int BufSize();
	
	//Функция, возвращающая максимальное количество элементов в буфере
	UFUNCTION(BlueprintCallable)
	int GetMaxSize();
};
