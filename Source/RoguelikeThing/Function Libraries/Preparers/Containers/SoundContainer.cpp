// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/Containers/SoundContainer.h"

void USoundContainer::AddSound(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString SoundTag, USoundWave* Sound)
{
    UE_LOG(LogTemp, Log, TEXT("ModuleName: %s, CategoryName: %s, SubCategoryName: %s, OwnerObjectName: %s, SoundTag: %s"), *ModuleName, *CategoryName, *SubCategoryName, *OwnerObjectName, *SoundTag);
    FSoundModule& Module = SoundsArray.FindOrAdd(ModuleName);
    FSoundCategory& Category = Module.SoundCategorysArray.FindOrAdd(CategoryName);
    FSoundSubCategory& SubCategory = Category.SoundSubCategorysArray.FindOrAdd(SubCategoryName);
    FSpecificObjectSounds& OwnerObject = SubCategory.SpecificObjectSoundsArray.FindOrAdd(OwnerObjectName);

    OwnerObject.SoundTagsArray.Add(SoundTag, Sound);
}

USoundWave* USoundContainer::FindSound(FString ModuleName, FString CategoryName, FString SubCategoryName, FString OwnerObjectName, FString SoundTag)
{
    FSoundModule* Module = SoundsArray.Find(ModuleName);
    if (Module) {
        FSoundCategory* Category = Module->SoundCategorysArray.Find(CategoryName);
        if (Category) {
            FSoundSubCategory* SubCategory = Category->SoundSubCategorysArray.Find(SubCategoryName);
            if (SubCategory) {
                FSpecificObjectSounds* Owner = SubCategory->SpecificObjectSoundsArray.Find(OwnerObjectName);
                if (Owner) {
                    USoundWave** Sound = Owner->SoundTagsArray.Find(SoundTag);
                    if (Sound) {
                        return *Sound;
                    }
                    else {
                        UE_LOG(LogTemp, Error, TEXT("!Sound"));
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
