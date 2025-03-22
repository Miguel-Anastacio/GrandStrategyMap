// Copyright 2024 An@stacioDev All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "UObject/Interface.h"
#include "WidgetCollectionInterface.generated.h"

class UWidgetTree;
class UWidget;

/**
* Blueprint-exposable interface for widget collections.
* This interface defines methods for managing collections of widgets that can display 
* UObjects and structs within container widgets.
*/
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UWidgetCollectionInterface: public UInterface
{
   GENERATED_BODY()
};

/**
* Interface implementation class for widget collections.
* Provides methods to manage, display, and manipulate collections of widgets.
*/
class DATABASEDWIDGET_API IWidgetCollectionInterface
{
   GENERATED_BODY() 

public:
   /**
    * Creates the container widget that will hold collection items.
    * Only available in the editor.
    */
#if WITH_EDITOR
   void CreateCollectionContainer();
#endif
   
   /**
    * Sets the widget class to use for individual items in the collection.
    * @param MembersWidgetClass - The widget class to use for collection items
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
   void SetWidgetItemClass(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr);
   
   /**
    * Implementation required for SetWidgetItemClass.
    * Pure virtual function that must be implemented by classes using this interface.
    * @param MembersWidgetClass - The widget class to use for collection items
    */
   virtual void SetWidgetItemClass_Implementation(TSubclassOf<UUserWidget> MembersWidgetClass = nullptr) = 0;
   
   /**
    * Determines if the collection container is a ListView widget.
    * @return True if the collection is a ListView, false otherwise
    */
   bool IsCollectionListView();
   
   /**
    * Gets the widget container that holds the collection items.
    * Must be implemented in BP/C++ 
    * @return The UWidget that serves as the container for collection items
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
   UWidget* GetCollectionContainer();

protected:
   /**
    * Sets the root widget for the collection.
    * Editor-only function for setting up the widget hierarchy.
    * @param Tree - The widget tree to use for setting up the collection
    */
#if WITH_EDITOR
   virtual void SetRootWidget(UWidgetTree* Tree) {};
#endif
};


UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UPropGenWidgetDataTableInterface: public UInterface
{
	GENERATED_BODY()
};

class DATABASEDWIDGET_API IPropGenWidgetDataTableInterface
{
	GENERATED_BODY()
public:
	/**
   * Sets the data table that provides data for the widget collection.
   * Optional method to override if the widget display depends on a DataTable.
   * @param NewDataTable - The data table to use for the widget collection
   */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
	void SetDataTable(UDataTable* NewDataTable);
};