// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BinArrayWrapper.generated.h"

/**
 * ���������, ����������� ������ ����������� ������ �� �������� ������� ��������
 */

USTRUCT()
struct FBinArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TArray<uint8> BinArray;
};
