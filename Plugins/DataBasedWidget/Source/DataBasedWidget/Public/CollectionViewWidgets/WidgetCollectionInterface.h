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
    * Clears all items from the widget collection.
    * Blueprint implementable for custom behavior in child widgets.
    */
   UFUNCTION(BlueprintCallable, DisplayName="Clear Collection", BlueprintImplementableEvent, Category = WidgetCollectionInterface)
   void BpClearCollection();

   /**
    * Native C++ implementation for clearing the collection.
    * Calls BpClearCollection
    */
   virtual void ClearCollection();

   /**
    * Adds a UObject to the widget collection.
    * @param Object - The UObject to add to the collection
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void AddObject(UObject* Object);
   
   /**
    * Adds an instanced struct to the widget collection.
    * @param Struct - The instanced struct to add to the collection
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void AddStruct(const FInstancedStruct& Struct);
   
   /**
    * Removes a specific UObject from the widget collection.
    * @param Object - The UObject to remove from the collection
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void RemoveObject(UObject* Object);
   
   /**
    * Removes a specific instanced struct from the widget collection.
    * @param Struct - The instanced struct to remove from the collection
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void RemoveStruct(const FInstancedStruct& Struct);
   
   /**
    * Initializes the collection with an array of instanced structs.
    * @param Structs - Array of instanced structs to initialize the collection with
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void InitFromStructs(const TArray<FInstancedStruct>& Structs);
   
   /**
    * Initializes the collection with an array of UObjects.
    * @param Objects - Array of UObjects to initialize the collection with
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = WidgetCollectionInterface)
   void InitFromObjects(const TArray<UObject*>& Objects);

   /**
    * Sets the data table that provides data for the widget collection.
    * Optional method to override if the widget display depends on a DataTable.
    * @param NewDataTable - The data table to use for the widget collection
    */
   UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category=WidgetCollectionInterface)
   void SetDataTable(UDataTable* NewDataTable);

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