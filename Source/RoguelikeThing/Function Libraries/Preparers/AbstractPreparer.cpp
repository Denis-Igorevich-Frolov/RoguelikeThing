// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/Preparers/AbstractPreparer.h"
#include <HAL/FileManagerGeneric.h>
#include <Kismet/GameplayStatics.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include <RoguelikeThing/SaveGame/DataSaver.h>

DEFINE_LOG_CATEGORY(AbstractPreparer);

//Проверка существования объектов из модуля Default. При обнаружении их отсутствия, они восстанавливаются из исходного списка
void UAbstractPreparer::CheckingDefaultAbstractObjects (UAbstractList* ObjectsList, FString ModuleRoot)
{
    IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/") + ModuleRoot + TEXT("/Default/xml"));
    //Если директории модуля Default нет, она создаётся
    if (!FileManager.DirectoryExists(*ModulePath)) {
        if (!FileManager.CreateDirectoryTree(*ModulePath)) {
            UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the CheckingDefaultAbstractObjects function - Failed to create directory %s"), *ModulePath);
        }

        UE_LOG(AbstractPreparer, Log, TEXT("Directory %s has been created"), *ModulePath);
    }

    TArray<FString> XML_Files = ObjectsList->GetModuleFilesArray();

    for (FString FileName : XML_Files) {
        FString FilePath = FString(ModulePath + "/" + FileName);

        //Если файла, связанного с объектом, который должен быть в модуле Default нет, то он восстанавливается из исходного списка
        if (!FileManager.FileExists(*FilePath)) {
            FString FileContent = *ObjectsList->GetXmlText(*FileName);

            if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the CheckingDefaultAbstractObjects function - Failed to create file %s"), *FilePath);
            }

            UE_LOG(AbstractPreparer, Log, TEXT("File %s from the Default module has been created"), *FilePath);
        }
    }
}

//Функция загрузки данных об объекте из его xml файла
template<typename DataType, typename ListType, typename PreparerType>
DataType* UAbstractPreparer::LoadDataFromXML(PreparerType* Preparer, FString ModuleName, FString ModuleRoot, FString XMLFilePath, IPlatformFile& FileManager,
    ListType* ObjectsList, DataType*(*UploadingData)(PreparerType* Preparer, DataType* Data, FXmlNode* RootNode, FString FileName, ListType* ObjectsList,
        FString ModuleName, FString XMLFilePath, IPlatformFile& FileManager, bool IsModDir, int RecursionDepth), bool IsModDir, int RecursionDepth)
{
    TArray<FString> PathPieces;
    //Переданный путь до xml файла разбивается на фрагменты, где последний элемент - это сам файл, а все предыдущие - папки, в которые он вложен
    XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);

    if (PathPieces.Num() == 0) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - The path to the xml file %s is empty or incorrect"), *XMLFilePath);
        FGenericPlatformMisc::RequestExit(false);
    }

    FString FileName = "";
    //Имя файла следует находить только для проверки на существование и валидность объекта из модуля Default
    if (ModuleName == "Default" && !IsModDir) {
        TArray<FString> FileNamePieces;
        //Берётся последний фрагмент пути до файла, который, непосредственно, будет полным именем самого файла, и разбивается на 2 части - имя и расширение
        PathPieces.Last().ParseIntoArray(FileNamePieces, TEXT("."), false);

        if (FileNamePieces.IsValidIndex(0)) {
            /* Из всего пути до файла вычленяется имя файла без расширения, которое используется как индекс
             * для поиска и, при необходимости, восстановления данных об объекте из модуля Default */
            FileName = FileNamePieces[0];
        }
        else {
            UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - File name %s has no extension or is incorrect"), *XMLFilePath);
            FGenericPlatformMisc::RequestExit(false);
        }
    }

    UPROPERTY()
    DataType* AbstractData = nullptr;

    //Безопасное создание UObject'ов гарантировано только в основном потоке
    AsyncTask(ENamedThreads::GameThread, [&AbstractData, this]() {
        AbstractData = NewObject<DataType>();
        /* Так как изначально AbstractData равна nullptr, сборщик мусора то и дело наравит вмешаться
         * в жизненный цикл переменной, так что её следует зарутировать от него подальше */
        AbstractData->AddToRoot();
        });

    //А пока Saver инициализируется в основном потоке, асинхронный ждёт, пока переменная не будет готова
    while (!AbstractData) {
        FPlatformProcess::SleepNoStats(0.0f);
    }

    UPROPERTY()
    FXmlFile XmlFile(*XMLFilePath);

    //Проверяется также и IsValid у XmlFile потому что переменная может нормально создаться, а файл, связанный с ней не загрузиться
    if (!XmlFile.IsValid()) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - xml code from file %s is not valid"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    UPROPERTY()
    FXmlNode* RootNode = XmlFile.GetRootNode();
    if (!RootNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract RootNode from file %s"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    AbstractData->Module = ModuleName;

    FXmlNode* IdNode = RootNode->FindChildNode("id");
    if (!IdNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract id node from file %s"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    FString id = IdNode->GetContent();
    if (id == "") {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Id node from file %s is empty"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    AbstractData->id = id;

    FXmlNode* CategoryNode = RootNode->FindChildNode("Category");
    if (!CategoryNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract Category node from file %s"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    FString Category = CategoryNode->GetContent();
    AbstractData->Category = Category;

    FXmlNode* SubCategoryNode = RootNode->FindChildNode("SubCategory");
    if (!SubCategoryNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract SubCategory node from file %s"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    FString SubCategory = SubCategoryNode->GetContent();
    AbstractData->SubCategory = SubCategory;

    FXmlNode* NameNode = RootNode->FindChildNode("Name");
    if (!NameNode) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Failed to extract Name node from file %s"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }

    FString Name = NameNode->GetContent();
    if (Name == "") {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - Name node from file %s is empty"), *XMLFilePath);

        EmergencyResetOfPointers<DataType>(AbstractData);

        if (ModuleName == "Default" && !IsModDir) {
            UE_LOG(AbstractPreparer, Log, TEXT("An attempt is made to restore default file %s"), *XMLFilePath);
            if (RestoringDefaultFileByName(FileName, ModuleRoot, ObjectsList)) {
                UE_LOG(AbstractPreparer, Log, TEXT("The attempt to recover file %s was successful"), *XMLFilePath);
                return LoadDataFromXML(Preparer, ModuleName, ModuleRoot, XMLFilePath, FileManager, ObjectsList, UploadingData, IsModDir, ++RecursionDepth);
            }
            else {
                UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the LoadDataFromXML function - An attempt to restore file %s failed. Abnormal termination."), *XMLFilePath);
                FGenericPlatformMisc::RequestExit(false);
            }
        }

        FGenericPlatformMisc::RequestExit(false);
    }
    AbstractData->Name = Name;

    DataType* Data = UploadingData(Preparer, AbstractData, RootNode, FileName, ObjectsList, ModuleName, XMLFilePath, FileManager, IsModDir, RecursionDepth);

    //Data->id += FString::FromInt(FDateTime::Now().GetMillisecond());

    return Data;
}

//Функция восстановления файла из моодуля Default по его имени (без расширения)
bool UAbstractPreparer::RestoringDefaultFileByName(FString Name, FString ModuleRoot, UAbstractList* ObjectsList)
{
    Name += ".xml";

    FString ModulePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/") + ModuleRoot + TEXT("/Default/xml/"));
    FString FilePath = ModulePath + Name;
    FString FileContent = ObjectsList->GetXmlText(*Name);

    if (FileContent == "") {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the RestoringDefaultFileByName function - The recoverable content of file %s of module %s is empty"), *Name, *ModuleRoot);
        return false;
    }

    if (!FFileHelper::SaveStringToFile(FileContent, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8, &IFileManager::Get(), EFileWrite::FILEWRITE_None)) {
        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the RestoringDefaultFileByName function - Failed to save content to file %s"), *FilePath);
        return false;
    }

    UE_LOG(AbstractPreparer, Log, TEXT("File %s has been restored"), *FilePath);
    return true;
}

template<typename DataType>
void UAbstractPreparer::EmergencyResetOfPointers(DataType* Data)
{
    if (Data && Data->IsValidLowLevel()) {
        if (Data->IsRooted())
            Data->RemoveFromRoot();

        Data->MarkPendingKill();
    }
}

//Функция получения данных обо всех объектах конкретного типа всех модулей
template<typename Container, typename DataType, typename PreparerType>
void UAbstractPreparer::GetAllData(Container* DataContainer, TArray<FString> ModsDirWithAbstract, UAbstractList* ObjectsList, FString ModuleRoot, PreparerType* Preparer,
    void(*AdditionaCalculations)(Container* Cont))
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [AdditionaCalculations, DataContainer, ModsDirWithAbstract, ObjectsList, ModuleRoot, Preparer, this]() {
        //Сначала проверяется не повреждены ли данные модуля Default
        CheckingDefaultAbstractObjects(ObjectsList, ModuleRoot);

        //Получение списка папок всех модулей
        TArray<FString> Dirs;
        FString StartDirPath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + TEXT("Data/") + ModuleRoot);
        FFileManagerGeneric::Get().FindFilesRecursive(Dirs, *StartDirPath, TEXT("*"), false, true);

        TArray<FString> ModuleNames;
        //Так как функция FindFilesRecursive находит все вложенные директории, а не только корневые, все остальные стоит отсеять
        for (FString Dir : Dirs) {
            //Путь до директории разбивается на массив последовательно вложенных папок
            TArray<FString> DirPieces;
            Dir.ParseIntoArray(DirPieces, TEXT("/"), false);

            //И для всех модулей проверяется на каком месте находится корень
            int SequentialDirectoryNumber = 0;
            DirPieces.Find(ModuleRoot, SequentialDirectoryNumber);

            /* Отсееваются некорневые директории по длине пути. Если директория корневая,
             * то длинна её пути должна быть равна последовательному номеру корня общего для
             * всех модулей + 1. Также стоит учеть, что SequentialDirectoryNumber - это индекс,
             * которому до последовательного номера следует прибавить ещё 1 */
            if (DirPieces.Num() == SequentialDirectoryNumber + 2 && DirPieces.IsValidIndex(SequentialDirectoryNumber + 1)) {
                ModuleNames.Add(*DirPieces[SequentialDirectoryNumber + 1]);
            }
        }

        //В конец вписка модулей добавляются моды
        ModuleNames.Append(ModsDirWithAbstract);

        IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
        for (FString ModuleName : ModuleNames) {
            UE_LOG(AbstractPreparer, Log, TEXT("Loading of the %s module %s has begun"), *ModuleRoot, *ModuleName);

            //Логика для загрузки обычных модулей и модовых немного отличается, так что необходимо знать какой модуль сейчас загружается
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

            //У путей xml файлов отбрасывается начало вплоть до директории проекта. Это необходимо для построения относительных путей файлов
            for (FString& XMLFilePath : XMLFilesPaths) {
                XMLFilePath = XMLFilePath.Replace(*FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectDir())), *FString(""));
            }

            UPROPERTY()
            UDataSaver* Saver = nullptr;
            //Безопасное создание UObject'ов гарантировано только в основном потоке
            AsyncTask(ENamedThreads::GameThread, [&Saver, this]() {
                Saver = NewObject<UDataSaver>();
                Saver->AddToRoot();
                });

            while (!Saver) {
                FPlatformProcess::SleepNoStats(0.0f);
            }

            FString DataSaverFilePath = "";
            //Путь до файлов мода чуть более сложный. Сам ModuleName у модов представлен как "Имя мода/Тип изменяемых объектов/Модуль"
            if (IsModDir) {
                TArray<FString> NamePieces;
                ModuleName.ParseIntoArray(NamePieces, TEXT("/"), false);
                //Конечное название sav переменной у модов даётся по имени изменяемого модуля объектов, а не всего модуля мода
                DataSaverFilePath = FPaths::ProjectDir() + "Mods/" + ModuleName + "/sav/" + NamePieces.Last() + ".sav";

                ModuleName = NamePieces.Last();
            }
            else {
                DataSaverFilePath = FPaths::ProjectDir() + "Data/" + ModuleRoot + "/" + ModuleName + "/sav/" + ModuleName + ".sav";
            }
            bool DataSaverFileExist = FileManager.FileExists(*DataSaverFilePath);

            //Если существует сериализованный файл данных о модуле, он загружается
            if (DataSaverFileExist) {
                Saver->LoadBinArray(DataSaverFilePath);
            }

            //Идёт проверка не изменилось ли количество xml файлов в директории модуля и не изменился ли их хеш, а так же не был ли файл sav перемещён
            if (Saver && XMLFilesPaths.Num() == Saver->GetBinArraySize() && Saver->CheckHashChange() &&
                DataSaverFilePath == Saver->GetSavFilePath()) {
                UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from a save file %s with serialized data"), *DataSaverFilePath);
                AsyncTask(ENamedThreads::GameThread, [ModuleName, IsModDir, this]() {
                    ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + ModuleName + ".sav"), FColor::FromHex("160124"));
                    });

                //Если все файлы остались неизменными с момента сериализации, то загрузка объектов ведётся напрямую из сохранённых бинарных данных
                TMap<FString, DataType*> AbstractData;

                AbstractData = Saver->GetData<DataType>(true);

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
                        UE_LOG(AbstractPreparer, Error, TEXT("!!! An error occurred in the AbstractPreparer class in the GetAllAbstractData function - AbstractDataRef is not valid"));
                        
                        TArray<FString> PathPieces;
                        XMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                        AsyncTask(ENamedThreads::GameThread, [ModuleName, PathPieces, this]() {
                            ChangeTextOfTheDownloadDetails.Broadcast(FString("Object " + PathPieces.Last().Left(4) + " could not be loaded"), FColor::Red);
                            });
                    }
                }
            }
            else {
                UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from an array of xml files"));

                //Если файлы менялись, то загрузка производится из xml файлов с их последующим сохранениев в новый sav файл
                Saver->ClearArray();
                for (FString XMLFilePath : XMLFilesPaths) {
                    FString FullXMLFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + XMLFilePath;

                    UE_LOG(AbstractPreparer, Log, TEXT("Loading will be done from a xml file %s"), *FullXMLFilePath);

                    TArray<FString> PathPieces;
                    FullXMLFilePath.ParseIntoArray(PathPieces, TEXT("/"), false);
                    AsyncTask(ENamedThreads::GameThread, [PathPieces, this]() {
                        ChangeTextOfTheDownloadDetails.Broadcast(FString("Loading file:  " + PathPieces.Last()), FColor::FromHex("160124"));
                        });

                    //Загрузка xml файла производится через функцию дочернего класса
                    UPROPERTY()
                    DataType* AbstractData = Preparer->LoadObjectFromXML(ModuleName, FullXMLFilePath, FileManager, IsModDir, 0);

                    if (AbstractData && AbstractData->IsValidLowLevel()) {
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

                //Заново сгенерированный массив данных обо всех объектах интеракции данного модуля сериализуется и сохраняется в sav файл
                Saver->SaveBinArray(DataSaverFilePath);

                if (Saver->IsValidLowLevel() && Saver->IsRooted())
                    Saver->RemoveFromRoot();
            }
        }

        if (AdditionaCalculations)
            AdditionaCalculations(DataContainer);

        AsyncTask(ENamedThreads::GameThread, [this]() {
            LoadingComplete.Broadcast();
            });
        });
}
