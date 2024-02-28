// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/*********************************************************************************************************
 * ������ ����� �������� ������� ������� ��� ��������� �������� ������ ��� ���������� ���������� ����
 *********************************************************************************************************/

//������ ������� ����������� ��� ���� ����
UENUM(BlueprintType)
enum class ELogType : uint8 {
    NONE          UMETA(DisplayName = "None"),
    REGULAR       UMETA(DisplayName = "Regular"),
    DETAILED      UMETA(DisplayName = "Detailed"),
};

UCLASS()
class ROGUELIKETHING_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
    virtual void Shutdown() override;
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELogType LogType = ELogType::REGULAR;
};
