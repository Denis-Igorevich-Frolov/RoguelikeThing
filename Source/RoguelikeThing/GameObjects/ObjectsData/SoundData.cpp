// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/SoundData.h"

void USoundData::Init(FString moduleName, FString categoryName, FString subCategoryName, FString itemName, FString soundTag,
	uint16 numChannels, uint32 sampleRate, uint16 bitsPerSample, uint32 dataSize, const uint8* pcmData)
{
	ModuleName = moduleName;
	CategoryName = categoryName;
	SubCategoryName = subCategoryName;
	ItemName = itemName;
	SoundTag = soundTag;

	NumChannels = numChannels;
	SampleRate = sampleRate;
	BitsPerSample = bitsPerSample;
	DataSize = dataSize;
	PCMData.Append(pcmData, dataSize);
}
