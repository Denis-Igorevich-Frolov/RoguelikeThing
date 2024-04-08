// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "NeighbourhoodOfCell.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNeighbourhoodOfCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnRight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnTop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnBottom = false;

	/* ����������, ���������� �� ���� �� ��������� ������������������� ��� ������� ����� ����������� �� ���������.
	 * ����������������������� ���������� ��������� ����������� � ������������ ��������� ��� ������ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInit = false;

	/* ����������� �� ��������� ������ ����������, � ������ IsInit ����� false. ����� ��������� ����� ���������
	 * �����������, �� ������� ��������� � ����� ����� ���������� ���������� IsInit �������� true ������� */
	FNeighbourhoodOfCell();
	FNeighbourhoodOfCell(bool SomethingOnRight, bool SomethingOnLeft, bool SomethingOnTop, bool SomethingOnBottom);
};
