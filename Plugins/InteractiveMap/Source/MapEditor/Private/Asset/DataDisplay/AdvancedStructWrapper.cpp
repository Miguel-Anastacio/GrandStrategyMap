// Copyright 2024 An@stacioDev All rights reserved.
#include "Asset/DataDisplay/AdvancedStructWrapper.h"
#include "Asset/DataDisplay/STreeJsonDisplay.h"
#include "FileIO/DataManagerFunctionLibrary.h"
#include "Log/LogFunctionLibrary.h"
#include "StructUtils/Public/InstancedStruct.h"
void UAdvancedStructWrapper::OpenFileOfType(const TArray<FString>& FileNames, TSharedPtr<STreeJsonDisplay>& Tree)
{
    TArray<FTestAdvanced> JsonData = UDataManagerFunctionLibrary::LoadCustomDataFromJson<FTestAdvanced>(FileNames[0]);
    ULogFunctionLibrary::LogArray(JsonData, "JsonData");

    FInstancedStruct InstancedStruct;
    // InstancedStruct.GetScriptStruct()
    TArray<const void*> Data;
    Data.Reserve(JsonData.Num());
    for (const auto& Item : JsonData)
    {
        Data.Add(&Item);
    }
	   
    if(Tree)
    {
        // Tree->RebuildTree(FTestAdvanced::StaticStruct(), Data);
    }
}

