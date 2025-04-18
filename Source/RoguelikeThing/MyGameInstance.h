// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/*********************************************************************************************************
 * Данный класс является базовым классом для менеджера высокого уровня для экземпляра запущенной игры
 *********************************************************************************************************/

//Уровни степени логирования для всей игры
UENUM(BlueprintType)
enum class ELogType : uint8 {
    NONE          UMETA(DisplayName = "None"),
    REGULAR       UMETA(DisplayName = "Regular"),
    DETAILED      UMETA(DisplayName = "Detailed"),
};

USTRUCT(BlueprintType)
struct FCurrentCallingItemInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemModule;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemCategory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemSubCategory;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemID;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int ItemQuantity;
};

UCLASS()
class ROGUELIKETHING_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
    virtual void Shutdown() override;
    virtual void Init() override;
    virtual void OnStart() override;
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELogType LogType = ELogType::REGULAR;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCurrentCallingItemInfo CurrentCallingItemInfo;

    UFUNCTION(BlueprintImplementableEvent)
    void AddItemToInventory(const FString& ItemModule, const FString& ItemCategory, const FString& ItemSubCategory, const FString& ItemID, int Quantity);
    UFUNCTION(BlueprintImplementableEvent)
    void MakeObjectUsed();
};
