// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <RoguelikeThing/SaveGame/DataSaver.h>

DEFINE_LOG_CATEGORY(AbstractPreparer);

//�������� ������������� �������� �� ������ Default. ��� ����������� �� ����������, ��� ����������������� �� ��������� ������
void UAbstractPreparer::CheckingDefaultAbstracts (UAbstractList* ObjectsList)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml"));
    //���� ���������� �������� �������������� �� ��������� ���, ��� ��������
    if (!FileManager.DirectoryExists(*ModulePath)) {
        if (!FileManager.CreateDirectoryTree(*ModulePath)) {
            UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the CheckingDefaultAbstract function - Failed to create directory %s"), *ModulePath);
        }

        UE_LOG(AbstractPreparer, Log, TEXT("Directory %s has been created"), *ModulePath);
    }

    TArray<FString> XML_Files;
    ObjectsList->GetXmlList().GenerateKeyArray(XML_Files);

    for (FString FileName : XML_Files) {
        FString FilePath = FString(ModulePath + "/" + FileName);

        //���� �����, ���������� � ��������, ������� ������ ���� � ������ Default ���, �� �� ����������������� �� ��������� ������
        if (!FileManager.FileExists(*FilePath)) {
            FString FileContent = *ObjectsList->GetXmlList().Find(*FileName);

            if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the CheckingDefaultAbstract function - Failed to create file %s"), *FilePath);
            }

            UE_LOG(AbstractPreparer, Log, TEXT("File %s from the Default module has been created"), *FilePath);
        }
    }
}

//������� �������� ������ �� ������� �� ��� xml �����
template<typename DataType, typename ListType, typename PreparerType>
DataType* UAbstractPreparer::LoadDataFromXML(PreparerType* Preparer, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, ListType* ObjectsList,
    DataType*(*UploadingData)(PreparerType* Preparer, DataType* Data, FXmlNode* RootNode, FString FileName, ListType* ObjectsList, FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, int RecursionDepth), int RecursionDepth)
{
    TArray<FString> PathPieces;
    //���������� ���� �� xml ����� ����������� �� ���������, ��� ��������� ������� - ��� ��� ����, � ��� ���������� - �����, � ������� �� ������
    XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);

    if (PathPieces.Num() == 0) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - The path to the xml file %s is empty or incorrect"), *XMLFilePath);
        return nullptr;
    }

    FString FileName = "";
    //��� ����� ������� �������� ������ ��� �������� �� ������������� � ���������� ������� �� ������ Default
    if (ModuleName == "Default") {
        TArray<FString> FileNamePieces;
        //������ ��������� �������� ���� �� �����, �������, ���������������, ����� ������ ������ ������ �����, � ����������� �� 2 ����� - ��� � ����������
        PathPieces.Last().ParseIntoArray(FileNamePieces, TEXT("."), false);

        if (FileNamePieces.IsValidIndex(0)) {
            /* �� ����� ���� �� ����� ����������� ��� ����� ��� ����������, ������� ������������ ��� ������
             * ��� ������ �, ��� �������������, �������������� ������ �� ������� �� ������ Default */
            FileName = FileNamePieces[0];
        }
        else {
            UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - File name %s has no extension or is incorrect"), *XMLFilePath);
            return nullptr;
        }
    }

    UPROPERTY()
    DataType* AbstractData = nullptr;

    AsyncTask(ENamedThreads::GameThread, [&AbstractData, this]() {
        AbstractData = NewObject<DataType>();
        AbstractData->AddToRoot();
        });

    while (!AbstractData) {
        FPlatformProcess::SleepNoStats(0.0f);
    }

    UPROPERTY()
    FXmlFile* XmlFile = new FXmlFile(*XMLFilePath);
    if (!XmlFile) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to create file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        //���� ���� ��� �� ������ Default, �� ������������ ������� ��� �������������� �� ��������� ������
        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                //� ����� ��� ��������������, ���������� ����������� ��� �� ������� � ����� ��� ��� ���������� ��������� ��� ��������������� ����
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                //���� �� ���� ������������ �� ������, ������������ ��������� ���������� ���������
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    if (!XmlFile->IsValid()) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - xml code from file %s is not valid"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    UPROPERTY()
    FXmlNode* RootNode = XmlFile->GetRootNode();
    if (!RootNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract RootNode from file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FXmlNode* IdNode = RootNode->FindChildNode("id");
    if (!IdNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract id node from file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString id = IdNode->GetContent();
    if (id == "") {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Id node from file %s is empty"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }
    AbstractData->id = id;

    FXmlNode* CategoryNode = RootNode->FindChildNode("Category");
    if (!CategoryNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract Category node from file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString Category = CategoryNode->GetContent();
    AbstractData->Category = Category;

    FXmlNode* SubCategoryNode = RootNode->FindChildNode("SubCategory");
    if (!SubCategoryNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract SubCategory node from file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString SubCategory = SubCategoryNode->GetContent();
    AbstractData->SubCategory = SubCategory;

    FXmlNode* NameNode = RootNode->FindChildNode("Name");
    if (!NameNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract Name node from file %s"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }

    FString Name = NameNode->GetContent();
    if (Name == "") {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Name node from file %s is empty"), *XMLFilePath);

        if (AbstractData && AbstractData->IsValidLowLevel()) {
            if (AbstractData->IsRooted())
                AbstractData->RemoveFromRoot();

            AbstractData->MarkPendingKill();
        }

        if (ModuleName == "Default") {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, XMLFilePath, FileManager, ObjectsList, UploadingData, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        return nullptr;
    }
    AbstractData->Name = Name;

    DataType* Data = UploadingData(Preparer, AbstractData, RootNode, FileName, ObjectsList, ModuleName, XMLFilePath, FileManager, RecursionDepth);

    if (XmlFile)
        delete XmlFile;

    return Data;
}

//������� �������������� ����� �� ������� Default �� ��� ����� (��� ����������)
bool UAbstractPreparer::RestoringDefaultFileByName(FString Name, UAbstractList* ObjectsList)
{
    Name += ".xml";

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects/Default/xml/"));
    FString FilePath = ModulePath + Name;
    const FString* FileContentRef = ObjectsList->GetXmlList().Find(*Name);

    if (!FileContentRef) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!!!!!!!!"));
        return false;
    }

    FString FileContent = *FileContentRef;

    if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the ConvertRelativePathToFull function - Failed to create file %s"), *FilePath);
        return false;
    }

    UE_LOG(AbstractPreparer, Log, TEXT("File %s has been restored"), *FilePath);
    return true;
}

//������� ��������� ������ ��� ���� �������� �������������� ���� �������
template<typename Container, typename DataType, typename PreparerType>
void UAbstractPreparer::GetAllData(Container* DataContainer, TArray<FString> ModsDirWithAbstract, UAbstractList* ObjectsList, PreparerType* Preparer)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [DataContainer, ModsDirWithAbstract, ObjectsList, Preparer, this]() {
        //������� ����������� �� ���������� �� ������ ������ Default
        CheckingDefaultAbstracts(ObjectsList);

        //��������� ������ ����� ���� �������
        TArray<FString> Dirs;
        FString StartDirPath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/Expedition interaction objects"));
        FFileManagerGeneric::Get().FindFilesRecursive(Dirs, *StartDirPath, TEXT("*"), false, true);

        TArray<FString> ModuleNames;
        //��� ��� ������� FindFilesRecursive ������� ��� ��������� ����������, � �� ������ ��������, ��� ��������� ����� �������
        for (FString Dir : Dirs) {
            //���� �� ���������� ����������� �� ������ ��������������� ��������� �����
            TArray<FString> DirPieces;
            Dir.ParseIntoArray(DirPieces, TEXT("/"), false);

            //� ����������� �� ����� ����� ��������� ������ ��� ���� ������� �������� ��������������
            int SequentialDirectoryNumber = 0;
            DirPieces.Find("Expedition interaction objects", SequentialDirectoryNumber);

            /* ����������� ���������� ���������� �� ����� ����. ���� ���������� ��������,
             * �� � ������ ������ ���� ����� ����������������� ������ ����� ������ ��� ����
             * ������� + 1. ����� ����� �����, ��� SequentialDirectoryNumber - ��� ������,
             * �������� �� ����������������� ������ ������� ��������� ��� 1 */
            if (DirPieces.Num() == SequentialDirectoryNumber + 2 && DirPieces.IsValidIndex(SequentialDirectoryNumber + 1)) {
                ModuleNames.Add(*DirPieces[SequentialDirectoryNumber + 1]);
            }
        }

        ModuleNames.Append(ModsDirWithAbstract);

        IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
        for (FString ModuleName : ModuleNames) {
            UE_LOG(AbstractPreparer, Log, TEXT("Loading of the expedition interaction objects module %s has begun"), *ModuleName);

            bool IsModDir = false;
            if (FileManager.DirectoryExists(*FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "Mods/" + ModuleName))) {
                IsModDir = true;
            }

            FString ModuleXMLDir = "";
            if (IsModDir) {
                ModuleXMLDir = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "Mods/" + ModuleName + "/xml");
            }
            else {
                ModuleXMLDir = FString(StartDirPath + "/" + ModuleName + "/xml");
            }

            TArray<FString> XMLFilesPaths;
            FileManager.FindFiles(XMLFilesPaths, *ModuleXMLDir, TEXT("xml"));

            for (FString& XMLFilePath : XMLFilesPaths) {
                XMLFilePath = XMLFilePath.Replace(*FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())), *FString(""));
            }

            UPROPERTY()
            UDataSaver* Saver = nullptr;

            AsyncTask(ENamedThreads::GameThread, [&Saver, this]() {
                Saver = NewObject<UDataSaver>();
                Saver->AddToRoot();
                });

            while (!Saver) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            FString DataSaverFilePath = "";
            if (IsModDir) {
                TArray<FString> NamePieces;
                ModuleName.ParseIntoArray(NamePieces, TEXT("/"), false);

                DataSaverFilePath = FPaths::ProjectDir() + "Mods/" + ModuleName + "/sav/" + NamePieces.Last() + ".sav";

                ModuleName = NamePieces.Last();
            }
            else {
                DataSaverFilePath = FPaths::ProjectDir() + "Data/Expedition interaction objects/" + ModuleName + "/sav/" + ModuleName + ".sav";
            }
            bool DataSaverFileExist = FileManager.FileExists(*DataSaverFilePath);

            //���� ���������� ��������������� ���� ������ � ������, �� �����������
            if (DataSaverFileExist) {
                Saver->LoadBinArray(DataSaverFilePath);
            }

            //��� �������� �� ���������� �� ���������� xml ������ � ���������� ������ � �� ��������� �� �� ���
            if (Saver && XMLFilesPaths.Num() == Saver->GetBinArraySize() && Saver->CheckHashChange() &&
                DataSaverFilePath == Saver->GetSavFilePath()) {
                UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from a save file %s with serialized data"), *DataSaverFilePath);
                AsyncTask(ENamedThreads::GameThread, [ModuleName, IsModDir, this]() {
                    ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + ModuleName + ".sav"), FColor::FromHex("160124"));
                    });

                //���� ��� ����� �������� ����������� � ������� ������������, �� �������� �������� ������ �������� �� ���������� �������� ������
                TMap<FString, DataType*> AbstractData;

                bool done = false;
                AsyncTask(ENamedThreads::GameThread, [&AbstractData, &Saver, &done]() {
                    AbstractData = Saver->GetData<DataType>();
                    done = true;
                    });

                while (!done) {
                    FPlatformProcess::SleepNoStats(0.0f);
                }

                for (FString XMLFilePath : XMLFilesPaths) {
                    DataType** AbstractDataRef = AbstractData.Find(XMLFilePath);

                    if (AbstractDataRef) {
                        UPROPERTY()
                        DataType* Data = *AbstractDataRef;
                        if (Data) {
                            DataContainer->AddData(ModuleName, Data->Category, Data->SubCategory, Data->id, Data);
                        }
                        else {
                            UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the GetAllAbstractData function - Data is not valid"));

                            TArray<FString> PathPieces;
                            XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                            AsyncTask(ENamedThreads::GameThread, [ModuleName, PathPieces, this]() {
                                ChangeTextOfTheDownloadDetails.Broadcast(FString("Object " + PathPieces.Last().Left(4) + " could not be loaded"), FColor::Red);
                                });
                        }
                    }
                    else {
                        UE_LOG(AbstractPreparer, Error, TEXT("!!!!!"));
                    }
                }
            }
            else {
                UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from an array of xml files"));

                //���� ����� ��������, �� �������� ������������ �� xml ������ � �� ����������� ����������� � ����� sav ����
                Saver->ClearArray();
                for (FString XMLFilePath : XMLFilesPaths) {
                    FString FullXMLFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + XMLFilePath;

                    UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from a xml file %s"), *FullXMLFilePath);

                    TArray<FString> PathPieces;
                    FullXMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                    AsyncTask(ENamedThreads::GameThread, [PathPieces, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces.Last()), FColor::FromHex("160124"));
                        });

                    UPROPERTY()
                    DataType* AbstractData = Preparer->LoadObjectFromXML(ModuleName, FullXMLFilePath, FileManager, 0);

                    if (AbstractData) {
                        Saver->AddDataToBinArray<DataType>(AbstractData, XMLFilePath, FullXMLFilePath);

                        DataContainer->AddData(ModuleName, AbstractData->Category, AbstractData->SubCategory, AbstractData->id, AbstractData);
                    }
                    else {
                        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the GetAllAbstractData function - AbstractData is not valid"));
                        AsyncTask(ENamedThreads::GameThread, [PathPieces, this]() {
                            ChangeTextOfTheDownloadDetails.Broadcast(FString("Object " + PathPieces.Last().Left(4) + " could not be loaded"), FColor::Red);
                            });
                    }
                }

                //������ ��������������� ������ ������ ��� ���� �������� ���������� ������� ������ ������������� � ����������� � sav ����
                Saver->SaveBinArray(DataSaverFilePath);

                if (Saver->IsValidLowLevel() && Saver->IsRooted())
                    Saver->RemoveFromRoot();
            }
        }
        AsyncTask(ENamedThreads::GameThread, [this]() {
            ChangeTextOfTheDownloadDetails.Broadcast(FString("Data loading complite"), FColor::Green);
            LoadingComplete.Broadcast();
            });
        });
}
