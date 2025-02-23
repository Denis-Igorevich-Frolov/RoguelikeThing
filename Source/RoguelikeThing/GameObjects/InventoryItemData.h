// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "RoguelikeThing/GameObjects/AbstractData.h"
#include "InventoryItemData.generated.h"

/**
 * 
 */

 //!!!!!�� ������ ������ ����������� � ��������� ���������� � ������������ ��� ���������� �������, ���������� � � ������� ������������
USTRUCT(BlueprintType)
struct FUseEvent
{
	GENERATED_BODY()
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UInventoryItemData : public UAbstractData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, SaveGame)
	int MaximumAmountInStack;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool IsUsable;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool AllowedForUseOnInventoryItemss;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	TMap<FString, FUseEvent> UseEvents;
};
