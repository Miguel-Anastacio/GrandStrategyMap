// Copyright 2024 An@stacioDev All rights reserved.
#pragma once
#include "CustomAssetActions.h"

#include "AssetToolsModule.h"
#include "EditorUtilityLibrary.h"
#include "GenericStructWidget.h"
#include "GenericWidgetDataMap.h"
#include "WidgetBlueprintEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "WidgetBlueprintFactory.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/UserWidgetBlueprint.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "Editor/UMGEditor/Public/WidgetBlueprint.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UMyAssetAction::UMyAssetAction()
{
	SupportedClasses.Add(UObject::StaticClass());
	// SupportedClasses.Add(UMyOtherAsset::StaticClass());
}

void UMyAssetAction::CreateWidgetFromObject() const 
{
	const TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (const UObject* Asset : SelectedAssets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Custom Action executed on: %s"), *Asset->GetName());
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());

		// Get Asset Class
		UStruct* AssetClass = GetAssetStruct(Asset);
		
		// create widget map
		UWidgetMapDataAsset* WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, Asset->GetName());
		if(WidgetMapDataAsset)
			WidgetMapDataAsset->CreateFromData(AssetClass, DefaultWidgetForFields);
		
		// create WBP_widget
		UBlueprint* GenericWidget = CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
		
		FString Message;
		UAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
	}
}

UWidgetMapDataAsset* UMyAssetAction::CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName)
{
	UObject* Asset = UAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath,UWidgetMapDataAsset::StaticClass(),
														"/DA_WidgetMapDataAsset_"  + ObjectOriginName);
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UWidgetMapDataAsset>(Asset);
}

UGenericStructWidget* UMyAssetAction::CreateWidgetFromObject(const FString& PackagePath, const FString& ObjectOriginName)
{
	UObject* Asset = UAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UGenericStructWidget::StaticClass(),
														"/WBP_GenericWidget_");
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UGenericStructWidget>(Asset);
}


UBlueprint* UMyAssetAction::CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName,
																UWidgetMapDataAsset* MapDataAsset)
{
	// Ensure the asset name is unique
	FString OutPackageName = AssetName;
	FString UniqueAssetName = AssetName;
	FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + TEXT("/WBP_GenericWidget_") + AssetName, TEXT(""), OutPackageName, UniqueAssetName);

	// Create package
	UPackage* Package = CreatePackage(*OutPackageName);
	if (!Package)
	{
		return nullptr;
	}

	// Create the blueprint class derived from UGenericStructWidget
	UBlueprint* NewBlueprint = FKismetEditorUtilities::CreateBlueprint(
		UGenericStructWidget::StaticClass(),   // Parent class
		Package,                              // Asset package
		*FPaths::GetBaseFilename(UniqueAssetName), // Asset name
		BPTYPE_Normal,                         // Normal Blueprint
		UWidgetBlueprint::StaticClass(),             // Base Blueprint class
		UWidgetBlueprintGeneratedClass::StaticClass(), // Generated class type
		FName("CreateAsset")                   // Unique transaction name
	);
	
	if (NewBlueprint)
	{
		FAssetRegistryModule::AssetCreated(NewBlueprint);
		Package->MarkPackageDirty();

		// **Modify the default field in UGenericStructWidget**
		if (UBlueprintGeneratedClass* BPGeneratedClass = Cast<UBlueprintGeneratedClass>(NewBlueprint->GeneratedClass))
		{
			if (UGenericStructWidget* DefaultObject = Cast<UGenericStructWidget>(BPGeneratedClass->GetDefaultObject()))
			{
				DefaultObject->CreateGenericWidget(MapDataAsset);
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBlueprint);
			}
		}
	}

	return NewBlueprint;
}

UStruct* UMyAssetAction::GetAssetStruct(const UObject* Asset)
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
		// For other assets, check if it's already a UScriptStruct
		// const UStruct* AssetStruct = Cast<UScriptStruct>(Asset);
  //       if(const UStruct* AssetStruct = Cast<UScriptStruct>(Asset))
  //       {
  //       	return AssetStruct;
  //       }
  //       else
  //       {
		return Asset->GetClass();
        // }
	}
}
