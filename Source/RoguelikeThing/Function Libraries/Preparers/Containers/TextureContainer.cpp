// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/Containers/TextureContainer.h"

DEFINE_LOG_CATEGORY(TextureContainer);

void UTextureContainer::AddTexture(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString TextureTag, UTexture2D* Texture)
{
    UE_LOG(LogTemp, Log, TEXT("ModuleName: %s, CategoryName: %s, SubCategoryName: %s, OwnerObjectName: %s, TextureTag: %s"), *ModuleName, *CategoryName, *SubCategoryName, *OwnerObjectName, *TextureTag);
    FTextureModule& Module = TexturesArray.FindOrAdd(ModuleName);
    FTextureCategory& Category = Module.TextureCategorysArray.FindOrAdd(CategoryName);
    FTextureSubCategory& SubCategory = Category.TextureSubCategorysArray.FindOrAdd(SubCategoryName);
    FSpecificObjectTextures& OwnerObject = SubCategory.SpecificObjectTexturesArray.FindOrAdd(OwnerObjectName);

    OwnerObject.TextureTagsArray.Add(TextureTag, Texture);
}

UTexture2D* UTextureContainer::FindTexture(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString TextureTag)
{
    FTextureModule* Module = TexturesArray.Find(ModuleName);
    if (Module) {
        FTextureCategory* Category = Module->TextureCategorysArray.Find(CategoryName);
        if (Category) {
            FTextureSubCategory* SubCategory = Category->TextureSubCategorysArray.Find(SubCategoryName);
            if (SubCategory) {
                FSpecificObjectTextures* Owner = SubCategory->SpecificObjectTexturesArray.Find(OwnerObjectName);
                if (Owner) {
                    UTexture2D** Texture = Owner->TextureTagsArray.Find(TextureTag);
                    if (Texture) {
                        return *Texture;
                    }
                    else {
                        UE_LOG(LogTemp, Error, TEXT("!Texture"));
                        return nullptr;
                    }
                }
                else {
                    UE_LOG(LogTemp, Error, TEXT("!Owner"));
                    return nullptr;
                }
            }
            else {
                UE_LOG(LogTemp, Error, TEXT("!SubCategory"));
                return nullptr;
            }
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("!Category"));
            return nullptr;
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("!Module"));
        return nullptr;
    }
}
