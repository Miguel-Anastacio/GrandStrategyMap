// Copyright 2024 An@stacioDev All rights reserved.
#include "Editor/CreateMapUIElements.h"
#include "CreateWidgetAssetAction.h"
#include "UserWidgets/DoubleTextBlock.h"
#include "EditorUtilityLibrary.h"
#include "MapObject.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "BlueprintLibrary/AssetCreatorFunctionLibrary.h"
#include "GenericWidget/GenericWidgetDataMap.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UI/Widgets/CustomButtonWidget.h"
#include "UI/Widgets/MapModeSelectorWidget.h"
#define LOCTEXT_NAMESPACE "CustomAssetActions"

#undef LOCTEXT_NAMESPACE
UCreateMapUIElements::UCreateMapUIElements()
{
	SupportedClasses.Add(UMapObject::StaticClass());
}

void UCreateMapUIElements::CreateMapDataWidgets(TSubclassOf<UUserWidget> DefaultWidgetForFields) const
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (UObject *Asset : SelectedAssets)
	{
		const UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return;
		
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		CreateWidgetForStruct(MapObject->StructType, PackagePath, DefaultWidgetForFields);
		CreateWidgetForStruct(MapObject->OceanStructType, PackagePath, DefaultWidgetForFields);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
	
}

void UCreateMapUIElements::CreateMapModeSelectorWidget(
	TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons) const
{
	const TArray<UObject *> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	for (UObject *Asset : SelectedAssets)
	{
		UMapObject* MapObject = Cast<UMapObject>(Asset);
		if (!MapObject)
			return;
		FString PackagePath = FPaths::GetPath(Asset->GetPathName());
		CreateMapModeWidget(MapObject, PackagePath, DefaultWidgetForButtons);
	}
	FString Message;
	UAtkAssetCreatorFunctionLibrary::SaveModifiedAssets(true, Message);
}

void UCreateMapUIElements::CreateAll(TSubclassOf<UUserWidget> DefaultWidgetForFields,
	TSubclassOf<class UCustomButtonWidget> DefaultWidgetForButtons) const
{
	CreateMapDataWidgets(DefaultWidgetForFields);
	CreateMapModeSelectorWidget(DefaultWidgetForButtons);
}

void UCreateMapUIElements::CreateWidgetForStruct(UScriptStruct* Struct, const FString& PackagePath, TSubclassOf<UUserWidget> DefaultWidgetForFields)
{
	UStruct *AssetClass = Struct;
	// create widget map
	if (UPropGenWidgetMapDataAsset *WidgetMapDataAsset = UCreateWidgetFromAssetAction::CreateWidgetMapDataAsset(PackagePath, AssetClass->GetName()))
	{
		if (!DefaultWidgetForFields)
		{
			DefaultWidgetForFields = UWPropGenDoubleTextBlock::StaticClass();
		}

		WidgetMapDataAsset->CreateFromData(AssetClass, DefaultWidgetForFields);
		// create WBP_widget
		UCreateWidgetFromAssetAction::CreateBlueprintDerivedFromGenericStructWidget(PackagePath, AssetClass->GetName(), WidgetMapDataAsset);
		UE_LOG(LogTemp, Warning, TEXT("Create Widget from Asset succeeded on: %s"), *AssetClass->GetName());
	}
}

void UCreateMapUIElements::CreateMapModeWidget(UMapObject* MapObject, const FString& PackagePath,
	TSubclassOf<UCustomButtonWidget> DefaultWidgetForButtons)
{
	const UStruct *AssetClass = MapObject->GetClass();
	if (!DefaultWidgetForButtons)
	{
		DefaultWidgetForButtons = UCustomButtonWidget::StaticClass();
	}
	
	UBlueprint *Blueprint = UAtkAssetCreatorFunctionLibrary::CreateBlueprintDerivedFromClass(PackagePath, UMapModeSelectorWidget::StaticClass(),
																							 TEXT("/WBP_MapModeSelector_") + AssetClass->GetName());
	if (!Blueprint)
	{
		return;
	}
	
	if (const UBlueprintGeneratedClass *BPGeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
	{
		if (UMapModeSelectorWidget *DefaultObject = Cast<UMapModeSelectorWidget>(BPGeneratedClass->GetDefaultObject()))
		{
			DefaultObject->MapModeSelectButton = DefaultWidgetForButtons;
			DefaultObject->MapObject = MapObject;
			DefaultObject->CreatePanelSlots();
			// FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
		}
	}
}
