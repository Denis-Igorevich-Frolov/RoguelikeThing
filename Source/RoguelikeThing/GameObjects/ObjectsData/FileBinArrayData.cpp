// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/GameObjects/ObjectsData/FileBinArrayData.h"

void UFileBinArrayData::Init(FString moduleName, FString categoryName, FString subCategoryName, FString itemName, FString textureTag, FFileBinary fileBinary)
{
	ModuleName = moduleName;
	CategoryName = categoryName;
	SubCategoryName = subCategoryName;
	ItemName = itemName;
	TextureTag = textureTag;
	FileBinary = fileBinary;
}
