// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CreateWidgetAssetAction.h"
#include "GenericWidget/GenericStructWidget.h"
#include "UserWidgets/DoubleTextBlock.h"
#include "EditorUtilityLibrary.h"
#include "GenericWidget/GenericWidgetDataMap.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/UserDefinedStruct.h"
#endif
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UCreateWidgetFromAssetAction::UCreateWidgetFromAssetAction()
{
	SupportedClasses.Add(UObject::StaticClass());
}

void UCreateWidgetFromAssetAction::CreateWidgetFromObject(TSubclassOf<UUserWidget> DefaultWidgetForFields) const
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (const UObject *Asset : SelectedAssets)
	{
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());

		// Get Asset Class
		UStruct *AssetClass = GetAssetStruct(Asset);

		// create widget map
		if (UPropGenWidgetMapDataAsset *WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, Asset->GetName()))
		{
			if (!DefaultWidgetForFields)
			{
				DefaultWidgetForFields = UWPropGenDoubleTextBlock::StaticClass();
			}

			WidgetMapDataAsset->CreateFromData(AssetClass, DefaultWidgetForFields);
			// create WBP_widget
			CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
			UE_LOG(LogTemp, Warning, TEXT("Create Widget from Asset succeeded on: %s"), *Asset->GetName());
		}
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

UPropGenWidgetMapDataAsset *UCreateWidgetFromAssetAction::CreateWidgetMapDataAsset(const FString &PackagePath, const FString &ObjectOriginName)
{
	UObject *Asset = UAtkAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UPropGenWidgetMapDataAsset::StaticClass(),
																						 TEXT("/DA_WidgetMapDataAsset_") + ObjectOriginName);
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UPropGenWidgetMapDataAsset>(Asset);
}

UBlueprint *UCreateWidgetFromAssetAction::CreateBlueprintDerivedFromGenericStructWidget(const FString &PackagePath, const FString &AssetName,
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

UStruct *UCreateWidgetFromAssetAction::GetAssetStruct(const UObject *Asset)
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
