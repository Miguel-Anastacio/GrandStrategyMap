// Copyright 2024 An@stacioDev All rights reserved.
#include "Asset/DataDisplay/BasicStructWrapper.h"
#include "Asset/DataDisplay/STreeJsonDisplay.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "Log/LogFunctionLibrary.h"
void UBasicStructWrapper::OpenFileOfType(const TArray<FString>& FileNames, TSharedPtr<STreeJsonDisplay>& Tree)
{
    if(FileNames.IsEmpty())
        return;
    
    TArray<FTestBasic> JsonData = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FTestBasic>(FileNames[0]);
    ULogFunctionLibrary::LogArray(JsonData, "JsonData");
    
    TArray<const void*> Data;
    Data.Reserve(JsonData.Num());
    for (const auto& Item : JsonData)
    {
        Data.Add(&Item);
    }
	   
    if(Tree)
    {
        Tree->RebuildTree(FTestBasic::StaticStruct(), Data);
    }
}

