// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "MyFileManager.generated.h"

/**************************************************************************************************************************
 * Данный класс является кастомным менеджером файлов, который проводит более сложные операции с ними, чем стандартный.
 * Например, отправляет файл во временную директорию или замещает временным файлом старый файл сохранения
 **************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MyFileManager, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMyFileManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;
	
public:
	UMyFileManager();

	//Функция, перемещающая указанный файл в указанную директорию внутри временной папки
	UFUNCTION(BlueprintCallable)
	bool CopyFileToTemporaryDirectory(FString PathToOriginalFile, FString PathToDirectoryInsideTempFolder, FString TemporaryFileName);

	/* Функция, сохраняющая временный файл в другую директорию. При необходимости заменяет файл
	 * на место которого сохраняется, старый файл при этом сохраняется в директорию бекапа */
	UFUNCTION(BlueprintCallable)
	bool SaveTempFileToOriginalDirectory(FString PathToOriginalFile, FString PathToTempFolder, FString PathToBackUp);
};
