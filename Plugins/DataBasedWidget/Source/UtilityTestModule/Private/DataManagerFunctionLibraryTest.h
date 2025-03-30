#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Engine/DataTable.h"
#include "DataManagerFunctionLibraryTest.generated.h"
// Helper struct for testing - must inherit from FTableRowBase for DataTable
USTRUCT()
struct FTestStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Value;

	FTestStruct() : Name(""), Value(0) {}
	FTestStruct(const FString& InName, int32 InValue) : Name(InName), Value(InValue) {}
	
	// Serialize to JSON
	void SerializeToJson(const TSharedPtr<FJsonObject>& OutJsonObject) const
	{
		OutJsonObject->SetStringField(TEXT("Name"), Name);
		OutJsonObject->SetStringField(TEXT("Value"), FString::FromInt(Value));
	}
    
	bool operator==(const FTestStruct& Other) const
	{
		return Name == Other.Name && Value == Other.Value;
	}
};

// Create a base class for shared test setup
class FAtkDataManagerTestBase
{
public:
	FString TestDir;
	FString TestJsonPath;
	UDataTable* TestDataTable;
	FTestStruct TestStruct;
	TArray<FTestStruct> TestArray;

	FAtkDataManagerTestBase() :
		 TestDir()
		,TestJsonPath()
		,TestDataTable(nullptr)
		,TestStruct()
		,TestArray()
	{
		CommonSetup();
	}
	virtual  ~FAtkDataManagerTestBase()
	{
		CommonTeardown();
	}
	// Common setup for all tests
	void CommonSetup()
	{
		// Setup test environment
		TestDir = FPaths::Combine(FPaths::AutomationTransientDir(), TEXT("AtkDataManagerTest"));
		IFileManager::Get().DeleteDirectory(*TestDir, false, true);
		IFileManager::Get().MakeDirectory(*TestDir);
        
		TestJsonPath = FPaths::Combine(TestDir, TEXT("TestData.json"));
        
		// Create test data
		TestDataTable = CreateMockDataTable();
		TestStruct.Name = TEXT("TestName");
		TestStruct.Value = 42;
        
		TestArray.Add(TestStruct);
		TestArray.Add(FTestStruct{TEXT("AnotherTest"), 84});
	}

	// Common teardown for all tests
	void CommonTeardown() const
	{
		IFileManager::Get().DeleteDirectory(*TestDir, false, true);
	}

	// Mock data table for testing
	static UDataTable* CreateMockDataTable() 
	{
		UDataTable* NewTestDataTable = NewObject<UDataTable>();
		NewTestDataTable->RowStruct = FTestStruct::StaticStruct();
        
		// Add test rows
		FTestStruct Row1;
		Row1.Name = TEXT("Test1");
		Row1.Value = 100;
		NewTestDataTable->AddRow(FName("Row1"), Row1);
        
		FTestStruct Row2;
		Row2.Name = TEXT("Test2");
		Row2.Value = 200;
		NewTestDataTable->AddRow(FName("Row2"), Row2);
        
		return NewTestDataTable;
	}

	TArray<FInstancedStruct> CreateMockInstancedStructsArray() const
	{
		TArray<FInstancedStruct> StructArray;
		for(const auto& InStruct : TestArray)
		{
			StructArray.Emplace(FInstancedStruct::Make(InStruct));
		}
		return StructArray;
	}
};
