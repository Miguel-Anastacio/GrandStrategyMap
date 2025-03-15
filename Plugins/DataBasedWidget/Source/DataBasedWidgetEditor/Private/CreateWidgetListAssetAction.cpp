// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CreateWidgetListAssetAction.h"

#include "EditorUtilityLibrary.h"
#include "GenericStructWidget.h"
#include "GenericWidgetDataMap.h"
#include "CollectionViewWidgets/CollectionViewWidgets.h"
#include "CollectionViewWidgets/CollectionViewWidgetsDataTable.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "CollectionViewWidgets/ListViewWidgetsDataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UCreateWidgetListAssetAction::UCreateWidgetListAssetAction()
{
	SupportedClasses.Add(UDataTable::StaticClass());
}

void UCreateWidgetListAssetAction::CreateWidgetListFromDataTable(TSubclassOf<UUserWidget> WidgetForListItems,
																TSubclassOf<UUserWidget> BaseViewWidget) const 
{
	const TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (const UObject* Asset : SelectedAssets)
	{
		const UDataTable* DataTable = Cast<UDataTable>(Asset);
		if(!DataTable)
			continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Custom Action executed on: %s"), *Asset->GetName());
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());

		// Get Struct of entries in data table 
		// User did not give a widget for entries create one
		if(!WidgetForListItems)
		{
			// create widget for item
			UStruct* ItemClass = Cast<UStruct>(DataTable->RowStruct);
			if(UWidgetMapDataAsset* WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, ItemClass->GetName()))
			{
				WidgetMapDataAsset->CreateFromData(ItemClass, DefaultWidgetForFields);
				// create WBP_widget
				UBlueprint* WidgetBlueprint = CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
				WidgetForListItems = WidgetBlueprint->GeneratedClass;
			}
		}
		UBlueprint* WidgetListBlueprint = CreateWidgetListBlueprint(PackagePath, Asset->GetName(), BaseViewWidget,
																	WidgetForListItems, DataTable);
		
	}
	FString Message;
	UAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

UWidgetMapDataAsset* UCreateWidgetListAssetAction::CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName)
{
	UObject* Asset = UAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UWidgetMapDataAsset::StaticClass(),
														TEXT("/DA_WidgetMapDataAsset_")  + ObjectOriginName);
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UWidgetMapDataAsset>(Asset);
}

UBlueprint* UCreateWidgetListAssetAction::CreateWidgetListBlueprint(const FString& PackagePath, const FString& ObjectOriginName,
																	TSubclassOf<UUserWidget> WidgetListBaseClass,
                                                                    TSubclassOf<UUserWidget> WidgetItemClass,
                                                                    const UDataTable* ListItems)
{
	UBlueprint* Blueprint = UAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, WidgetListBaseClass,
																						TEXT("/WBP_ListView_") + ObjectOriginName);
	if (Blueprint)
	{
		if (const UBlueprintGeneratedClass* BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		{
			if (UWPropGenCollectionViewDataTable* DefaultObject = Cast<UWPropGenCollectionViewDataTable>(BPGeneratedClass->GetDefaultObject()))
			{
				DefaultObject->CreateListView(WidgetItemClass);
				DefaultObject->SetDataTable(const_cast<UDataTable*>(ListItems));
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
			}
		}
	}
	return Blueprint;
}

UBlueprint* UCreateWidgetListAssetAction::CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName,
                                                                                        UWidgetMapDataAsset* MapDataAsset) const
{
	UBlueprint* Blueprint = UAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UGenericStructWidget::StaticClass(),
																						TEXT("/WBP_GenericWidget_") + AssetName);
	if (Blueprint)
	{
		// **Modify the default field in UGenericStructWidget**
		if (const UBlueprintGeneratedClass* BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		{
			if (UGenericStructWidget* DefaultObject = Cast<UGenericStructWidget>(BPGeneratedClass->GetDefaultObject()))
			{
				DefaultObject->CreateGenericWidget(MapDataAsset);
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
			}
		}
	}

	return Blueprint;
}

UStruct* UCreateWidgetListAssetAction::GetAssetStruct(const UObject* Asset)
{
	// Handle different asset types
	if (const UBlueprint* BlueprintAsset = Cast<UBlueprint>(Asset))
	{
		// Get the generated class from the blueprint
		return BlueprintAsset->GeneratedClass;
	}
	else if (const UUserDefinedStruct* UserStruct = Cast<UUserDefinedStruct>(Asset))
	{
		// For user-defined structs, get the actual struct
		return const_cast<UUserDefinedStruct*>(UserStruct);
	}
	else
	{
		return Asset->GetClass();
	}
}
