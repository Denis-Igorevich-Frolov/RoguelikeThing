// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RoguelikeThing/SaveGame/BinArrayWrapper.h"
#include "DataSaver.generated.h"

/**********************************************************************
 * ������ ����� �������� �������, ��������� ���� ���������� .sav
 * ��� �������� ������� ������ �������� �������������� ����������.
 **********************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(DataSaver, Log, All);

UCLASS()
class ROGUELIKETHING_API UDataSaver : public USaveGame
{
	GENERATED_BODY()
	
protected:

	/* ��������� �������� ������ �������� �������������� ����������, ������
	 * ������� �������� ���� �� ���������������� xml �����, � ��������� -
	 * ��������������� ������ ���������� �� ����� xml ����� */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FBinArrayWrapper> BinData;

	/* ��������� ����� xml ������ �������� �������������� ����������, ������ �������
	 * �������� ���� �� ���������������� xml �����, � ��������� - ��� ��� */
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FString, FString> XMLFilesHash;

	UPROPERTY(VisibleAnywhere, SaveGame)
	FString SavFilePath;

public:

	/* �������, ����������� ������ � �������� �������������� ���������� � ������ ���� ��������
	 * ���������������� ������ ��� ������������ ����������. ����� ����������� ��� ��������� xml ����� */
	template<typename Data> 
	void AddDataToBinArray(Data* ObjectData, FString SavedLocalFilePath, FString FullFilePath);
	/* ��������� ��������� ���� �������� �������������� ���������� ������������ ������. ������ ���������
	 * �������� ���� �� ��������� xml �����, � ��������� - ��������������� ������ �� ����� ������� */
	template<typename Data>
	TMap<FString, Data*> GetData();

	const FString GetSavFilePath();
	/* ��������� �������������� ������ �������� �������������� ����������, ����������� �
	 * ������ ������, ����������� � sav ����, ������� ����� ��������� �� ����������� ���� */
	void SaveBinArray(FString FilePath);
	/* �������� ������� �������� �������������� ����������, ������� ��� ������� � ���� �� ����������� ����.
	 * ����� ����� ������� ���� ������, ������� ��������������� �������� GetData */
	void LoadBinArray(FString FilePath);
	//�������� ��������� ����� xml �����, ��������� � ������� ����������� �������. true - ��������� ���, false - ����
	bool CheckHashChange();
	//��������� ���������� ���������, � �������������� � xml ������, �� ��������� ������� ���� ������������ ���������� � ���� ��������� ������
	int GetBinArraySize();
	//������ ������� ���� �������� ������� ������
	void ClearArray();
};
