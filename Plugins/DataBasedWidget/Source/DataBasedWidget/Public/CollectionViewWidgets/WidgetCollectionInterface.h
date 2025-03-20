// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "UObject/Interface.h"
#include "WidgetCollectionInterface.generated.h"

class UWidgetTree;
class UWidget;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UWidgetCollectionInterface: public UInterface
{
	GENERATED_BODY()
};

class DATABASEDWIDGET_API IWidgetCollectionInterface
{
	GENERATED_BODY()

public:
	// Is this needed ?
	UFUNCTION(BlueprintImplementableEvent, Category = WidgetCollectionInterface)
	void OnRefreshCollection();
	virtual void NativeRefreshCollection();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void ClearCollection();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void AddObject(UObject* Object);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void AddStruct(const FInstancedStruct& Struct);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void RemoveObject(UObject* Object);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void RemoveStruct(const FInstancedStruct& Struct);
	
	// wrapper to init from Instanced Struct exposed to BP
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void InitFromStructs(const TArray<FInstancedStruct>& Structs);
	
	// init from UObject exposed to BP
	UFUNCTION(BlueprintCallable,  BlueprintNativeEvent, Category = WidgetCollectionInterface)
	void InitFromObjects(const TArray<UObject*>& Objects);

	// Optional method to override if the Widget Display is dependent on a DataTable
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void SetDataTable(UDataTable* NewDataTable);

	// Function that creates the container for displaying a collection of widgets
#if WITH_EDITOR
	void CreateCollectionContainer();
#endif
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void SetWidgetItemClass(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr);
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr) = 0;
	
	// Determines if the Collection container is of type ListView
	bool IsCollectionListView();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	UWidget* GetCollectionContainer();

protected:
#if WITH_EDITOR
	virtual void SetRootWidget(UWidgetTree* Tree)
	{
		
	};
#endif

};
