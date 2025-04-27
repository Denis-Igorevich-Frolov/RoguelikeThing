// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SoundData.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API USoundData : public UObject
{
	GENERATED_BODY()
	
public:
	void Init(FString moduleName, FString categoryName, FString subCategoryName, FString itemName, FString soundTag,
		uint16 numChannels, uint32 sampleRate, uint16 bitsPerSample, uint32 dataSize, const uint8* pcmData);

	UPROPERTY(SaveGame)
	FString ModuleName;
	UPROPERTY(SaveGame)
	FString CategoryName;
	UPROPERTY(SaveGame)
	FString SubCategoryName;
	UPROPERTY(SaveGame)
	FString ItemName;
	UPROPERTY(SaveGame)
	FString SoundTag;

	UPROPERTY(SaveGame)
	uint16 NumChannels;
	UPROPERTY(SaveGame)
	uint32 SampleRate;
	UPROPERTY(SaveGame)
	uint16 BitsPerSample;
	UPROPERTY(SaveGame)
	uint32 DataSize;
	UPROPERTY(SaveGame)
	TArray<uint8> PCMData;
};
