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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void CreateCollectionContainer();
	virtual void CreateCollectionContainer_Implementation();
#endif
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void SetWidgetItemClass(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr);
	virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr) = 0;
	
	// Determines if the Collection container is of type ListView
	bool IsCollectionListView();

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void SetRootWidget(UWidgetTree* Tree);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	UWidget* GetCollectionContainer();

};
