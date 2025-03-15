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
UCreateWidgetFromAssetAction::UCreateWidgetFromAssetAction()
{
	SupportedClasses.Add(UObject::StaticClass());
	BaseGenericWidget = UGenericStructWidget::StaticClass();
}

void UCreateWidgetFromAssetAction::CreateWidgetFromObject() const 
{
	const TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (const UObject* Asset : SelectedAssets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Custom Action executed on: %s"), *Asset->GetName());
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());

		// Get Asset Class
		UStruct* AssetClass = GetAssetStruct(Asset);
		
		// create widget map
		if(UWidgetMapDataAsset* WidgetMapDataAsset = CreateWidgetMapDataAsset(PackagePath, Asset->GetName()))
		{
			WidgetMapDataAsset->CreateFromData(AssetClass, DefaultWidgetForFields);
			// create WBP_widget
			CreateBlueprintDerivedFromGenericStructWidget(PackagePath, Asset->GetName(), WidgetMapDataAsset);
		}
		
	}
	FString Message;
	UAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

UWidgetMapDataAsset* UCreateWidgetFromAssetAction::CreateWidgetMapDataAsset(const FString& PackagePath, const FString& ObjectOriginName)
{
	UObject* Asset = UAssetCreatorFunctionLibrary::CreateAssetInPackageWithUniqueName(PackagePath, UWidgetMapDataAsset::StaticClass(),
														TEXT("/DA_WidgetMapDataAsset_")  + ObjectOriginName);
	if (!Asset)
	{
		return nullptr;
	}
	return Cast<UWidgetMapDataAsset>(Asset);
}

UBlueprint* UCreateWidgetFromAssetAction::CreateBlueprintDerivedFromGenericStructWidget(const FString& PackagePath, const FString& AssetName,
																UWidgetMapDataAsset* MapDataAsset) const
{
	UBlueprint* Blueprint = UAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, BaseGenericWidget,
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

UStruct* UCreateWidgetFromAssetAction::GetAssetStruct(const UObject* Asset)
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
