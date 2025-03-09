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
		UClass* AssetClass = nullptr;
		if (const UBlueprint* BlueprintAsset = Cast<UBlueprint>(Asset))
		{
			// Get the generated class from the blueprint
			AssetClass = BlueprintAsset->GeneratedClass;
		}
		else
		{
			// Directly get the class for non-blueprint assets
			AssetClass = Asset->GetClass();
		}
		
		// create widget map
		UWidgetMapDataAsset* WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, Asset->GetName());
		if(WidgetMapDataAsset)
			WidgetMapDataAsset->CreateFromClass(AssetClass, DefaultWidgetForFields);
		
		// create WBP_widget
		UBlueprint* GenericWidget = CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
		// SetupBlueprintAfterCreation(GenericWidget, WidgetMapDataAsset);
		// if(GenericWidget)
			// GenericWidget->CreateGenericWidget(WidgetMapDataAsset);
		
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
	AssetToolsModule.Get().CreateUniqueAssetName(PackagePath + TEXT("/") + AssetName, TEXT(""), OutPackageName, UniqueAssetName);

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

				// Mark the Blueprint as structurally modified so the changes persist
				FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(NewBlueprint);
			}
		}
	}

	return NewBlueprint;
}

void UMyAssetAction::SetupBlueprintAfterCreation(UBlueprint* CreatedBlueprint, UWidgetMapDataAsset* WidgetMap)
{
	if (!CreatedBlueprint)
	{
		return;
	}
    
	// Open the Blueprint in the editor
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	AssetEditorSubsystem->OpenEditorForAsset(CreatedBlueprint);
    
	// Cast to the widget blueprint editor
	IAssetEditorInstance* EditorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(CreatedBlueprint, true);
	FWidgetBlueprintEditor* WidgetEditor = static_cast<FWidgetBlueprintEditor*>(EditorInstance);
	if (!WidgetEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to open widget editor"));
		return;
	}
    
	// Get the preview widget (this is the actual UGenericStructWidget instance in the editor)
	UUserWidget* PreviewWidget = WidgetEditor->GetPreview();
	if (UGenericStructWidget* GenericStructWidget = Cast<UGenericStructWidget>(PreviewWidget))
	{
		GenericStructWidget->CreateGenericWidget(WidgetMap);
		// Make sure changes are saved
		// WidgetEditor->();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get GenericStructWidget from preview"));
	}
}
