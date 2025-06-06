// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CreateWidgetListAssetAction.h"

#include "EditorUtilityLibrary.h"
#include "GenericWidget/GenericStructWidget.h"
#include "GenericWidget/GenericWidgetDataMap.h"
#include "CollectionViewWidgets/MutableCollectionObjectsView.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "CollectionViewWidgets/ListViewWidgetsDataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/UserDefinedStruct.h"
#endif
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UCreateWidgetListAssetAction::UCreateWidgetListAssetAction()
{
	SupportedClasses.Add(UDataTable::StaticClass());
}

void UCreateWidgetListAssetAction::CreateWidgetListFromDataTable(TSubclassOf<UUserWidget> WidgetForListItems,
																 TSubclassOf<UUserWidget> BaseViewWidget) const
{
	// If the user does not specify class then use default PropGenListView
	if (!BaseViewWidget)
	{
		BaseViewWidget = UWPropGenListViewDataTable::StaticClass();
	}
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (const UObject *Asset : SelectedAssets)
	{
		const UDataTable *DataTable = Cast<UDataTable>(Asset);
		if (!DataTable)
			continue;

		UE_LOG(LogTemp, Warning, TEXT("Custom Action executed on: %s"), *Asset->GetName());
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());

		// Get Struct of entries in data table
		// User did not give a widget for entries create one
		if (!WidgetForListItems)
		{
			// create widget for item
			UStruct *ItemClass = Cast<UStruct>(DataTable->RowStruct);
			if (UPropGenWidgetMapDataAsset *WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, ItemClass->GetName()))
			{
				WidgetMapDataAsset->CreateFromData(ItemClass, DefaultWidgetForFields);
				// create WBP_widget
				const UBlueprint *WidgetBlueprint = CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
				WidgetForListItems = WidgetBlueprint->GeneratedClass;
			}
		}
		UBlueprint *WidgetListBlueprint = CreateWidgetListBlueprint(PackagePath, Asset->GetName(), BaseViewWidget,
																	WidgetForListItems, DataTable);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

UPropGenWidgetMapDataAsset *UCreateWidgetListAssetAction::CreateWidgetMapDataAsset(const FString &PackagePath, const FString &ObjectOriginName)
{
	UObject *Asset = UAtkAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UPropGenWidgetMapDataAsset::StaticClass(),
																						 TEXT("/DA_WidgetMapDataAsset_") + ObjectOriginName);
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UPropGenWidgetMapDataAsset>(Asset);
}

UBlueprint *UCreateWidgetListAssetAction::CreateWidgetListBlueprint(const FString &PackagePath, const FString &ObjectOriginName,
																	TSubclassOf<UUserWidget> WidgetListBaseClass,
																	TSubclassOf<UUserWidget> WidgetItemClass,
																	const UDataTable *ListItems)
{
	if (!ValidateContainerClassAndMemberWidget(WidgetListBaseClass, WidgetItemClass))
	{
		return nullptr;
	}

	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, WidgetListBaseClass,
																							 TEXT("/WBP_ListView_") + ObjectOriginName);
	if (Blueprint)
	{
		if (const UBlueprintGeneratedClass *BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		{
			if (UObject *DefaultObject = BPGeneratedClass->GetDefaultObject())
			{
				if (DefaultObject->GetClass()->ImplementsInterface(UPropGenWidgetCollectionInterface::StaticClass()))
				{
					Cast<IPropGenWidgetCollectionInterface>(DefaultObject)->CreateCollectionContainer();
					IPropGenWidgetCollectionInterface::Execute_SetWidgetItemClass(DefaultObject, WidgetItemClass);
					IPropGenWidgetDataTableInterface::Execute_SetDataTable(DefaultObject, const_cast<UDataTable *>(ListItems));
					FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
				}
			}
		}
	}
	return Blueprint;
}

UBlueprint *UCreateWidgetListAssetAction::CreateBlueprintDerivedFromGenericStructWidget(const FString &PackagePath, const FString &AssetName,
																						UPropGenWidgetMapDataAsset *MapDataAsset) const
{
	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UWPropGenGeneric::StaticClass(),
																							 TEXT("/WBP_GenericWidget_") + AssetName);
	if (Blueprint)
	{
		// **Modify the default field in UGenericStructWidget**
		if (const UBlueprintGeneratedClass *BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		{
			if (UWPropGenGeneric *DefaultObject = Cast<UWPropGenGeneric>(BPGeneratedClass->GetDefaultObject()))
			{
				DefaultObject->CreateGenericWidget(MapDataAsset);
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
			}
		}
	}

	return Blueprint;
}

UStruct *UCreateWidgetListAssetAction::GetAssetStruct(const UObject *Asset)
{
	// Handle different asset types
	if (const UBlueprint *BlueprintAsset = Cast<UBlueprint>(Asset))
	{
		// Get the generated class from the blueprint
		return BlueprintAsset->GeneratedClass;
	}
	else if (const UUserDefinedStruct *UserStruct = Cast<UUserDefinedStruct>(Asset))
	{
		// For user-defined structs, get the actual struct
		return const_cast<UUserDefinedStruct *>(UserStruct);
	}
	else
	{
		return Asset->GetClass();
	}
}

bool UCreateWidgetListAssetAction::ValidateContainerClassAndMemberWidget(
	const TSubclassOf<UUserWidget> &WidgetListBaseClass, const TSubclassOf<UUserWidget> &WidgetItemClass)
{
	if (!WidgetListBaseClass || !WidgetItemClass)
		return false;

	if (IPropGenWidgetCollectionInterface *Interface = Cast<IPropGenWidgetCollectionInterface>(WidgetListBaseClass->ClassDefaultObject))
	{
		if (Interface->IsCollectionListView())
		{
			if (!WidgetItemClass->ImplementsInterface(UUserObjectListEntry::StaticClass()))
			{
				UE_LOG(LogTemp, Error, TEXT("Widget List Base Class is based on a List View but Widget for members does"
											"not implement UUserObjectListEntry"));
				return false;
			}
		}
	}

	return true;
}
