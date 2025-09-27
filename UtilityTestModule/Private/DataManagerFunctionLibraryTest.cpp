// Copyright 2024 An@stacioDev All rights reserved.
#include "DataManagerFunctionLibraryTest.h"
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Engine/DataTable.h"
#include "BlueprintLibrary/DataManagerFunctionLibrary.h"

// Test fixture for UAtkDataManagerFunctionLibrary
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataManagerFlJsonTest, "AnastacioUtilityToolkit.UtilityModule.DataManagerFL.Json", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDataManagerFlDataTableTest, "AnastacioUtilityToolkit.UtilityModule.DataManagerFL.DataTable", 
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FDataManagerFlDataTableTest::RunTest(const FString& Parameters)
{

    // Test ReadDataTableToArray with valid data table
    {
        const FAtkDataManagerTestBase TestBase;
        TArray<FTestStruct*> ResultArray;
        const bool Success = UAtkDataManagerFunctionLibrary::ReadDataTableToArray(TestBase.TestDataTable, ResultArray);
        TestTrue("ReadDataTableToArray should return true for valid data table", Success);
        TestEqual("ReadDataTableToArray should return correct number of rows", ResultArray.Num(), TestBase.TestDataTable->GetRowMap().Num());
    }
    
    // Test with null data table
    {
        const FAtkDataManagerTestBase TestBase;
        TArray<FTestStruct*> EmptyArray;
        const bool Success = UAtkDataManagerFunctionLibrary::ReadDataTableToArray(nullptr, EmptyArray);
        TestFalse("ReadDataTableToArray should return false for null data table", Success);
        TestTrue("Array should be empty", EmptyArray.IsEmpty());
    }
    
    // Test GetArrayOfInstancedStructs
    {
        const FAtkDataManagerTestBase TestBase;
        const TArray<FInstancedStruct> InstancedStructs = UAtkDataManagerFunctionLibrary::GetArrayOfInstancedStructs(TestBase.TestDataTable);
        TestEqual("GetArrayOfInstancedStructs should return correct number of elements", InstancedStructs.Num(), TestBase.TestDataTable->GetRowMap().Num());
    }
    
    return true;
}

bool FDataManagerFlJsonTest::RunTest(const FString& Parameters)
{
    // Test Write/Read InstancedStructs 
    {
        const FAtkDataManagerTestBase TestBase;
        const TArray<FInstancedStruct> InstancedStructs = TestBase.CreateMockInstancedStructsArray();
        UAtkDataManagerFunctionLibrary::WriteInstancedStructArrayToJson(TestBase.TestJsonPath, InstancedStructs);
        const TArray<FInstancedStruct> ReadFromFile = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson(TestBase.TestJsonPath, FTestStruct::StaticStruct());
        TestEqual("Structs written to file equal to read", InstancedStructs.Num(), ReadFromFile.Num());
    }
    
    // Test write/read TArray templated
    {
        const FAtkDataManagerTestBase TestBase;
        bool bResult = false;
        FString Message;
        UAtkDataManagerFunctionLibrary::WriteArrayToJsonFile(TestBase.TestJsonPath, TestBase.TestArray, bResult, Message);
        TestTrue("WriteArrayToJsonFile should return true for valid array", bResult);
        
        const TArray<FTestStruct> Data = UAtkDataManagerFunctionLibrary::LoadCustomDataFromJson<FTestStruct>(TestBase.TestJsonPath);
        TestEqual("Read from Json", Data.Num(), TestBase.TestArray.Num());
    }
    
    return true;
}